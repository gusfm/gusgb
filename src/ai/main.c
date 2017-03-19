#include <stdio.h>
#include <stdlib.h>
#include "gb_ai.h"

int main(int argc, char *argv[])
{
    const char *rom_path = argv[1];
    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <rom path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int zoom = 4;
    int ret = gb_ai_init(160 * zoom, 144 * zoom, (float)zoom, rom_path);
    if (ret < 0) {
        fprintf(stderr, "ERROR: could not load ROM %s\n", rom_path);
        exit(EXIT_FAILURE);
    }
    gb_ai_main();
    gb_ai_finish();
    return 0;
}
