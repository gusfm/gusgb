#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "gb_ai.h"

static void handler(int sig)
{
    printf("Caught signal %d\n", sig);
    gb_ai_finish();
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    const char *rom_path = argv[1];
    const char *ai_filename = argv[2];
    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <rom path> [ai file name]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* Set handler for CTRL + C */
    struct sigaction act;
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
    if (ai_filename != NULL) {
        ret = gb_ai_load(ai_filename);
        if (ret < 0) {
            exit(EXIT_FAILURE);
        }
    }
    gb_ai_main();
    gb_ai_finish();
    return 0;
}
