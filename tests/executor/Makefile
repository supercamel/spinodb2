CC=g++
CFLAGS=-c -g -Wall -std=c++14 -I../../ -I../../cppsrc
LDFLAGS= -g
SOURCES=$(wildcard *.cpp)  \
../../cppsrc/query_executor.cpp \
../../cppsrc/query_parse.cpp \
../../cppsrc/dom_node.cpp \
../../cppsrc/dom_arr.cpp \
../../cppsrc/dom_obj.cpp \
../../cppsrc/collection.cpp

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
