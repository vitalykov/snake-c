#include "util.h"

unsigned int RandomUnsignedInt(void) {
    unsigned int result;
    unsigned int* ptr = &result;
    result = (unsigned int)ptr;

    return result;
}

int Mod(int a, int b) { return ((a % b) + b) % b; }

int Abs(int a) { return a > 0 ? a : -a; }
