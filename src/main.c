#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_boy.h"

int main(int argc, char *argv[])
{
    bool debug = false;
    uint16_t breakpoint = 0x0000;
    const char *rom_path = argv[1];
    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <rom path> [options]\n", argv[0]);
        fprintf(stderr,
                "Options:\n -d\t\tenable debug\n -b <addr>\tadd breakpoint\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 2) {
        for (int i = 2; i < argc; ++i) {
            if (strcmp(argv[i], "-d") == 0) {
                debug = true;
            } else if (strcmp(argv[i], "-b") == 0) {
                breakpoint = (uint16_t)strtoul(argv[++i], NULL, 16);
            }
        }
    }
    printf("rom_path=%s, debug=%d, breakpoint=0x%04x\n", rom_path, debug,
           breakpoint);
    int zoom = 4;
    int ret = gb_init(160 * zoom, 144 * zoom, (float)zoom, rom_path, debug, breakpoint);
    if (ret < 0) {
        fprintf(stderr, "ERROR: could not load ROM %s\n", rom_path);
        exit(EXIT_FAILURE);
    }
    gb_main();
    gb_finish();
    return 0;
}
