#include <stdio.h>
#include <stdlib.h>
#include "game_boy.h"

int main(int argc, char *argv[])
{
    const char *rom_path = argv[1];
    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <rom path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int zoom = 4;
    int ret = gb_init(160 * zoom, 144 * zoom, (float)zoom, rom_path);
    if (ret < 0) {
        exit(EXIT_FAILURE);
    }
    gb_main();
    gb_finish();
    return 0;
}
