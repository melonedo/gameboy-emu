SRCS = $(wildcard */*.cpp)

CFLAGS = -Wall -std=c++11 -O1

CC = g++

PROGS = $(patsubst %.cpp,%,$(SRCS))

all: $(PROGS)

%: %.cpp
	$(CC) $(CFLAGS) -c -o $@.o $^
