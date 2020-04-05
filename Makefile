SRCS = \
	cpu/cpu.cpp \
	cpu/instruction-set.cpp \
	util/byte-type.cpp \
	util/bit-register.cpp \
	memory/memory.cpp \
	video/video.cpp \
	main/emu.cpp \
	main/window.cpp \
	main/threads.cpp \
	main/main.cpp

PROGS = $(patsubst %.cpp,%.o,$(SRCS))

CC = g++

CFLAGS = -Wall -std=c++11 -O0 -pthread -ggdb -Wno-format

LIBRARY_PATHS = -LD:\Mingw_Lib\lib

INCLUDE_PATHS = -ID:\MinGW_Lib\include\SDL2

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

all: $(PROGS)
	$(CC) $(CFLAGS) -o gameboy-emu $^ $(LIBRARY_PATHS) $(LINKER_FLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INCLUDE_PATHS)
