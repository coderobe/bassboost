#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <tclap/CmdLine.h>

static bool quiet = false;
std::mutex unhold_hold;

std::mutex ram_load_hold;
void ram_load_alloc(int amount){
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

std::mutex cpu_load_hold;
void cpu_load_hog(){
	while(!cpu_load_hold.try_lock()){}
	cpu_load_hold.unlock();
	return;
}

void unhold(){
	unhold_hold.unlock();
	if(!quiet) printf("Press Enter to exit at any time.\n");
	std::cin.get();
	ram_load_hold.unlock();
	cpu_load_hold.unlock();
	return;
}

int main(int argc, char** argv) {
	// Set up release mutex
	unhold_hold.lock();
	ram_load_hold.lock();

	// Set up TCLAP cmdline
	TCLAP::CmdLine BassBoost("BassBoost- a variable system load", ' ', "1.0");
	// Memory load CLI opt
	TCLAP::ValueArg<int> ram_load_arg("m", "memory", "Amount of ram load (in MiB) to generate", false, 0, "int");
	BassBoost.add(ram_load_arg);
	// CPU load CLI opt
	TCLAP::ValueArg<int> cpu_load_arg("c", "cpu", "Amount of cpu hog threads to spawn", false, 0, "int");
	BassBoost.add(cpu_load_arg);
	// Disable verbosity (default on)
	TCLAP::SwitchArg quiet_arg("q", "quiet", "Don't print anything", BassBoost, false);

	// Parse the argv array
	BassBoost.parse(argc, argv);

	// Verbosity check!
	quiet = quiet_arg.getValue();
	if(!quiet) printf("BassBoost- running in verbose mode\n");

	// Start unfreeze listener
	std::thread unhold_thread(unhold);
	unhold_hold.lock();


	// Memory load
	static const int ram_load = ram_load_arg.getValue();
	std::thread ram_load_thread;
	if(ram_load != 0){
		// Spawn new ram_load_alloc thread, passing our CLI opt ram_load
		ram_load_thread = std::thread(std::bind(ram_load_alloc, ram_load));
		if(!quiet) printf("Spawned ram load thread\n");
	}

	// CPU hog
	static const int cpu_load = cpu_load_arg.getValue();
	std::vector<std::thread> cpu_hogs;
	if(cpu_load != 0){
		// Lock our holding mutex, preventing the cpu_load_hog threads from suiciding immediately
		cpu_load_hold.lock();
		for(int hl = cpu_load; hl--; hl){
			if(!quiet) printf("Spawned cpu hog thread #%d\n", hl);
			// Push cpu_load amount of new cpu_load_hog threads to our vector
			cpu_hogs.push_back(std::thread(cpu_load_hog));
		}
	}

	// Join the threads, waiting for the mutex to unlock in unhold_thread
	if(ram_load_thread.joinable()){
		ram_load_thread.join();
		if(!quiet) printf("Freed RAM\n");
	}
	for(int hl = cpu_load; hl--; hl){
		if(cpu_hogs[hl].joinable()){
			//if(!quiet) printf("Joining cpu hog thread #%d\n", hl);
			// Join the last thread, waiting for the mutex to unlock in unhold_thread
			cpu_hogs.back().join();
			if(!quiet) printf("Killed CPU hog thread #%d\n", hl);
			// Pop dead threads
			cpu_hogs.pop_back();
		}
	}
	// Join keyboard listener if it's still alive (clean up)
	if(unhold_thread.joinable()) unhold_thread.join();
	if(!quiet) printf("Quitting...\n");

	return 0;
}
