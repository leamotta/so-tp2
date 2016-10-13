CC = g++
CFLAGS = -g -Wall -Werror -std=c++11
LDFLAGS = -pthread

BINARIES = Tester

all: $(BINARIES)

Tester: RWLock.cpp RWLock.h Tester.cpp
	$(CC) $(LDFLAGS) $(CFLAGS) Tester.cpp RWLock.cpp -o Tester

clean:
	rm -f *.o
	rm -f $(BINARIES)
