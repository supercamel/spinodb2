CC=g++
CFLAGS=-c -O3 -Wall -std=c++14 -I../../
LDFLAGS= -O3 
SOURCES=$(wildcard *.cpp) ../../query_executor.cpp ../../query_parse.cpp
HEADERS=$(wildcard *.h) $(../../*.h)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(HEADERS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)  $(HEADERS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	find . -name \*.o -execdir rm {} \;
	rm ../../*.o
	rm -f $(EXECUTABLE)
