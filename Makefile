CC = g++
CFLAGS = -g -std=c++14 -Wall
LDFLAGS =

all: test_all

test_all: json_test.o json.o
	$(CC) $(CFLAGS) $(LDFLAGS) json_test.o json.o -o test_all

json_test.o: json_test.cpp json.hpp
	$(CC) $(CFLAGS) -c json_test.cpp -o json_test.o

json.o: json.cpp json.hpp
	$(CC) $(CFLAGS) -c json.cpp -o json.o

clean:
	rm -f test_all json_test.o json.o