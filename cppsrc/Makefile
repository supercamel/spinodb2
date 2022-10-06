CC=g++
CFLAGS=-c -g -pg -Wall -std=c++14 -I../
LDFLAGS= -g -pg
SOURCES= main.cpp \
		 query_executor.cpp \
		 query_parse.cpp \
		 dom_node.cpp \
		 dom_arr.cpp \
		 dom_node.cpp \
		 database.cpp \
		 collection.cpp \
		 index.cpp

HEADERS=$(wildcard *.h)
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
