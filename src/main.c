#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "game_boy.h"

static int scale = 4;
static char *romfile = NULL;
static bool fullscreen = false;

static int parse_args(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "s:fch")) != -1) {
        switch (opt) {
            case 's':
                scale = strtol(optarg, NULL, 10);
                if (scale < 1 || scale > 10) {
                    fprintf(stderr, "Invalid scale: %d\n", scale);
                    return -1;
                }
                break;
            case 'f':
                fullscreen = true;
                break;
            case 'c':
                printf(
                    "%s:\n"
                    "Controls:\n"
                    "P:\t\tPause emulation\n"
                    "O:\t\tDump emulator debugs\n"
                    "ESC, Q:\t\tQuit program\n"
                    "Alt + Enter:\tToggle fullscreen mode\n"
                    "\n"
                    "Key mapping:\n"
                    "Up:\t\tup\n"
                    "Down:\t\tdown\n"
                    "Left:\t\tleft\n"
                    "Right:\t\tright\n"
                    "A:\t\ta\n"
                    "B:\t\ts\n"
                    "Start:\t\tenter\n"
                    "Select:\t\tleft shift\n",
                    argv[0]);
                exit(EXIT_SUCCESS);
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
            "  -f\t\tStart in fullscreen mode\n"
            "  -h\t\tPrint help and exit\n"
            "  -s <scale>\tScale video output\n",
            argv[0]);
}

int main(int argc, char *argv[])
{
    if (parse_args(argc, argv) < 0) {
        print_help(argv);
        exit(EXIT_FAILURE);
    }
    int ret = gb_init(scale, romfile, fullscreen);
    if (ret < 0) {
        exit(EXIT_FAILURE);
    }
    gb_main();
    gb_finish();
    return 0;
}
