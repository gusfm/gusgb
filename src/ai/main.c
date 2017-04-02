#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gb_ai.h"

static void handler(int sig)
{
    (void)sig;
    gb_ai_finish();
    exit(EXIT_SUCCESS);
}

static void print_help(const char *bin_name)
{
    fprintf(stderr, "Syntax: %s <rom path> [OPTIONS]\n", bin_name);
    fprintf(stderr, "OPTIONS:\n");
    fprintf(stderr, "\t-p: <population input file>\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    const char *ai_path = NULL;
    /* Parse args. */
    if (argc < 2) {
        print_help(argv[0]);
    }
    if (argc > 2) {
        if (strcmp(argv[2], "-p") == 0) {
            ai_path = argv[3];
            if (ai_path == NULL) {
                print_help(argv[0]);
            }
        } else {
            print_help(argv[0]);
        }
    }
    const char *rom_path = argv[1];
    /* Set handler for CTRL + C */
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = &handler;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }
    int zoom = 4;
    int ret = gb_ai_init(160 * zoom, 144 * zoom, (float)zoom, rom_path);
    if (ret < 0) {
        fprintf(stderr, "ERROR: could not load ROM %s\n", rom_path);
        exit(EXIT_FAILURE);
    }
    gb_ai_main(ai_path);
    gb_ai_finish();
    return 0;
}
