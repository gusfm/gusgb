#include <stdio.h>
#include <stdlib.h>
#include "game_boy.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <rom path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int ret = game_boy_init(argv[1]);
    if (ret < 0) {
        fprintf(stderr, "ERROR: could not load ROM %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    game_boy_main();
    return 0;
}
