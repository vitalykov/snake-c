#include "util.h"

#include <unistd.h>

unsigned int RandomUnsignedInt(void) {
    unsigned int result;
    getentropy(&result, sizeof(result));

    return result;
}

int Mod(int a, int b) { return ((a % b) + b) % b; }

int Abs(int a) { return a > 0 ? a : -a; }
