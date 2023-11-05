#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static void usage(const char *bin) {
    printf("Usage: %s [-b (bus)] [-a (all) | -r (rang)]\n", bin);
}

int main (int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}
