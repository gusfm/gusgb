#include <stdio.h>
#include <stdlib.h>
#include "game_boy.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <rom path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int ret = gb_init(argv[1], 800, 600, true);
    if (ret < 0) {
        fprintf(stderr, "ERROR: could not load ROM %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    gb_main();
    gb_finish();
    return 0;
}
