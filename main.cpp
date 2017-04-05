#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <mutex>
#include <tclap/CmdLine.h>

static bool quiet = false;
std::mutex unhold_hold;

std::mutex ram_load_hold;
void ram_load_alloc(int amount){
	//int amount = *amem;
	int amount_bytes = sizeof(unsigned char)*1024*1024*amount;
	if(!quiet) printf("Allocating... malloc(%dMiB)\n", amount);
	void* mem = malloc(amount_bytes);
	if(!quiet) printf("Filling heap...\n");
	memset(mem, 'a', amount_bytes);
	if(!quiet) printf("Heap full - waiting for release\n");
	ram_load_hold.lock();
	free(mem);
	return;
}

void unhold(){
	unhold_hold.unlock();
	if(!quiet) printf("Press Enter to exit at any time.\n");
	std::cin.get();
	ram_load_hold.unlock();
	return;
}

int main(int argc, char** argv) {
	// Set up release mutex
	unhold_hold.lock();
	ram_load_hold.lock();

	// Set up TCLAP cmdline
	TCLAP::CmdLine BassBoost("BassBoost- a variable system load", ' ', "0.1");

	// Memory load
	TCLAP::ValueArg<int> ram_load_arg("m", "memory", "Amount of ram load (in MiB) to generate", false, 0, "int");
	BassBoost.add(ram_load_arg);

	// Disable verbosity
	TCLAP::SwitchArg quiet_arg("q", "quiet", "Don't print anything", BassBoost, false);

	// Parse the argv array
	BassBoost.parse(argc, argv);

	quiet = quiet_arg.getValue();
	if(!quiet) printf("BassBoost- running in verbose mode\n");

	// Start unfreeze listener
	std::thread unhold_thread(unhold);
	unhold_hold.lock();

	int ram_load = ram_load_arg.getValue();
	if(ram_load != 0){
		std::thread ram_load_thread(std::bind(ram_load_alloc, ram_load));
		if(!quiet) printf("Spawned ram load thread\n");
		if(ram_load_thread.joinable()) ram_load_thread.join();
		//ram_load_alloc(ram_load);
	}

	// Join keyboard listener if it's still alive (clean up)
	if(unhold_thread.joinable()) unhold_thread.join();
	if(!quiet) printf("Quitting...\n");

	return 0;
}
