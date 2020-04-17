SRCS = \
	cpu/cpu.cpp \
	cpu/instruction-set.cpp \
	util/byte-type.cpp \
	util/bit-register.cpp \
	util/thread-util.cpp \
	memory/memory.cpp \
	video/video.cpp \
	main/emu.cpp \
	main/window.cpp \
	main/main.cpp

OBJS = $(patsubst %.cpp,%.o,$(SRCS))

PROG = gameboy-emu

CC = g++

CFLAGS = -Wall -std=c++11 -O1 -pthread -ggdb -Wno-format

LIBRARY_PATHS = -LD:\Mingw_Lib\lib

INCLUDE_PATHS = -ID:\MinGW_Lib\include\SDL2

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2


all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $^ $(LIBRARY_PATHS) $(LINKER_FLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INCLUDE_PATHS)

.ONESHELL:
cpu/instruction-set.cpp: cpu/gen-instruction-set.py cpu/instruction-data.json
	cd cpu && python gen-instruction-set.py

clean:
	rm -f $(OBJS)
	rm -f $(PROG)
	rm -f cpu/instruction-set.cpp
