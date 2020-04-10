# gameboy-emu
A gameboy emulator, nothing fancy

## Building
* Make sure you have G++, python, and GNU make are installed in your PATH.
* Download SDL and manually configure the `INCLUDE_PATHS` and `LIBRARY_PATHS` variables in `Makefile`.
* Run `make`.
* You may need to copy SDL binary `SDL2.dll` to this directory to run the compiled program.
