#include "debugger.h"

int main() {//j 1500
    cpu8080 i8080;
    reset(&i8080);
    load(&i8080, "invaders.h", 0);
    load(&i8080, "invaders.g", 0x800);
    load(&i8080, "invaders.f", 0x1000);
    load(&i8080, "invaders.e", 0x1800);
    debug(&i8080);
}
