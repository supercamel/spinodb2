CC=g++
CFLAGS=-c -g -Wall -std=c++14 -I../../cppsrc -I../../
LDFLAGS= -g
SOURCES=$(wildcard *.cpp) ../../cppsrc/collection.cpp ../../cppsrc/database.cpp ../../cppsrc/dom_node.cpp ../../cppsrc/index.cpp ../../cppsrc/query_executor.cpp ../../cppsrc/query_parse.cpp
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
	cd ../../cppsrc && rm *.o
	rm -f $(EXECUTABLE)
