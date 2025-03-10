#include "util.h"

#include <fcntl.h>
#include <unistd.h>

unsigned int RandomUnsignedInt(void) {
    unsigned int result;
    int fd = open("/dev/urandom", O_RDONLY);

    if (fd == -1) {
        return 0;
    }

    read(fd, &result, sizeof(result));

    return result;
}

int Mod(int a, int b) { return ((a % b) + b) % b; }

int Abs(int a) { return a > 0 ? a : -a; }
