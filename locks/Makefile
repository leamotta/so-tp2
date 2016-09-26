CC = g++
CFLAGS = -g -Wall -Werror -std=c++11
LDFLAGS = -pthread

%.o: %.cpp %.h
	$(CC) $(LDFLAGS) $(CFLAGS) -c $<

clean:
	rm -f *.o
