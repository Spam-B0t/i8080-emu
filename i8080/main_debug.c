#include "debugger.h"

int main() {//j 1500
    cpu8080 i8080;
    reset(&i8080);
    /*load(&i8080, "cpudiag.bin", 0x100);
    //modify start:
    i8080.memory[0x0005]=0xc3;
    i8080.memory[0x6]=0;
    i8080.memory[0x7]=0x01;
    //skip ac test:
    i8080.memory[0x59c] = 0xc3;
    i8080.memory[0x59d] = 0xc2;
    i8080.memory[0x59e] = 0x05;*/
    load(&i8080, "invaders.h", 0);
    load(&i8080, "invaders.g", 0x800);
    load(&i8080, "invaders.f", 0x1000);
    load(&i8080, "invaders.e", 0x1800);
    debug(&i8080);
}
