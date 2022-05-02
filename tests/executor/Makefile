CC=g++
CFLAGS=-c -g -Wall -std=c++14 -I../../
LDFLAGS= -g
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
	rm -f $(EXECUTABLE)
