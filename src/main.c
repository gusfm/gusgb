#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "game_boy.h"

int scale = 4;
char *romfile = NULL;

static int parse_args(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "s:ch")) != -1) {
        switch (opt) {
            case 's':
                scale = strtol(optarg, NULL, 10);
                if (scale < 1 || scale > 10) {
                    fprintf(stderr, "Invalid scale: %d\n", scale);
                    return -1;
                }
                break;
            case 'c':
                printf(
                    "Gusgb:\n"
                    "Controls:\n"
                    "P:\tPause emulation\n"
                    "O:\tDump emulator debugs\n"
                    "\n"
                    "Key mapping:\n"
                    "Up:\tup\n"
                    "Down:\tdown\n"
                    "Left:\tleft\n"
                    "Right:\tright\n"
                    "A:\ts\n"
                    "B:\ta\n"
                    "Start:\tenter\n"
                    "Select:\tspace\n"
                    );
                exit(EXIT_SUCCESS);
            case '?':
            case 'h':
            default:
                return -1;
        }
    }
    romfile = argv[optind];
    if (romfile == NULL) {
        return -1;
    }
    return 0;
}

static void print_help(char **argv)
{
    fprintf(stderr,
            "Usage: %s [options] romfile\n"
            "Options:\n"
            "  -c\t\tPrint keyboard controls\n"
            "  -s <scale>\tScale video output\n",
            argv[0]);
}

int main(int argc, char *argv[])
{
    if (parse_args(argc, argv) < 0) {
        print_help(argv);
        exit(EXIT_FAILURE);
    }
    int ret = gb_init(scale, romfile);
    if (ret < 0) {
        exit(EXIT_FAILURE);
    }
    gb_main();
    gb_finish();
    return 0;
}
