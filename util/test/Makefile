#OBJS specifies which files to compile as part of the project
# OBJS = test-bit-register.cpp

#Dependency
DEPS = ../util/bit-register.o ../util/byte-type.o ../memory/memory.o

#CC specifies which compiler we're using
CC = g++

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = test-bit-register test-add-signed test-memory-reference

#gcc has a hard time parsing hh and ll in formats
CFLAGS = -g -Wall -Wno-format

.cpp:
	$(CC) $(CFLAGS) $(DEPS) -o $@ $<

all : $(OBJ_NAME)
