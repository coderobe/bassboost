CC:=g++
# If Windows
#CC:=i686-w64-mingw32-g++
#LDFLAGS:=-static
CFLAGS:=-I. -I/usr/local/opt/tclap/include -std=c++11
LDFLAGS:=-L/usr/local/opt/tclap/lib -lpthread

bassboost:
	$(CC) -o bassboost $(CFLAGS) $(LDFLAGS) main.cpp

clean:
	rm bassboost
