OBJS	= main.o scanner.o testScanner.o
SOURCE	= main.cpp scanner.cpp testScanner.cpp
HEADER	= scanner.h
OUT	    = scanner
CC	    = g++
FLAGS	= -g -c -Wall

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS) -std=c++11

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp -std=c++11

scanner.o: scanner.cpp
	$(CC) $(FLAGS) scanner.cpp -std=c++11

testScanner.o: testScanner.cpp
	$(CC) $(FLAGS) testScanner.cpp -std=c++11

clean:
	rm -f $(OBJS) $(OUT)