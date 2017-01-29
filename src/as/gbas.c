#include "gbas.h"
#include <stdlib.h>
#include <unistd.h>

static void gbas_print_help(char **argv)
{
    fprintf(stderr, "Syntax: %s -i <input file> -o <output file> [options]\n",
            argv[0]);
    fprintf(stderr, "Options:\n\t-g generate cartridge header\n");
}

gbas_t *gbas_init(int argc, char **argv)
{
    int opt;
    gbas_t *gbas = malloc(sizeof(gbas_t));
    gbas->infile = NULL;
    gbas->outfile = NULL;
    gbas->gen_header = false;
    while ((opt = getopt(argc, argv, "i:o:g")) != -1) {
        switch (opt) {
            case 'i':
                gbas->infile = optarg;
                break;
            case 'o':
                gbas->outfile = optarg;
                break;
            case 'g':
                gbas->gen_header = true;
                break;
            default:
                gbas_print_help(argv);
                return NULL;
        }
    }
    if (gbas->infile == NULL) {
        fprintf(stderr, "No input file found!\n");
        gbas_print_help(argv);
        return NULL;
    } else if (gbas->outfile == NULL) {
        fprintf(stderr, "No output file found!\n");
        gbas_print_help(argv);
        return NULL;
    }
    gbas->input = fopen(gbas->infile, "r");
    if (!gbas->input) {
        fprintf(stderr, "Can't open input file %s\n", gbas->infile);
        return NULL;
    }
    gbas->output = fopen(gbas->outfile, "wb");
    if (gbas->output == NULL) {
        fprintf(stderr, "Can't open output file %s\n", gbas->outfile);
        fclose(gbas->input);
        return NULL;
    }
    return gbas;
}

void gbas_finish(gbas_t *gbas)
{
    fclose(gbas->input);
    fclose(gbas->output);
    free(gbas);
}
