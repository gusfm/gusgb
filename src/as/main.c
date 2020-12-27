#include <stdio.h>
#include <unistd.h>
#include "as/cart.h"
#include "parser.h"

FILE *output;

typedef struct {
    char *infile;
    char *outfile;
    bool gen_header;
} params_t;

static void print_help(char **argv)
{
    fprintf(stderr, "Syntax: %s -i <input file> -o <output file> [options]\n",
            argv[0]);
    fprintf(stderr, "Options:\n\t-g generate cartridge header\n");
}

static int read_params(int argc, char **argv, params_t *params)
{
    int opt;
    params->infile = NULL;
    params->outfile = NULL;
    params->gen_header = false;
    while ((opt = getopt(argc, argv, "i:o:g")) != -1) {
        switch (opt) {
            case 'i':
                params->infile = optarg;
                break;
            case 'o':
                params->outfile = optarg;
                break;
            case 'g':
                params->gen_header = true;
                break;
            default:
                print_help(argv);
                return -1;
        }
    }
    if (params->infile == NULL) {
        fprintf(stderr, "No input file found!\n");
        print_help(argv);
        return -1;
    } else if (params->outfile == NULL) {
        fprintf(stderr, "No output file found!\n");
        print_help(argv);
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    params_t params;
    parser_t parser;
    FILE *input;

    if (read_params(argc, argv, &params) != 0) {
        return -1;
    }

    input = fopen(params.infile, "r");
    if (input == NULL) {
        fprintf(stderr, "Could not open input file %s\n", params.infile);
        return -1;
    }

    output = fopen(params.outfile, "wb");
    if (output == NULL) {
        fprintf(stderr, "Could not open output file %s\n", params.outfile);
        return -1;
    }

    int ret = parser_init(&parser, input, output);
    if (ret != PARSER_OK) {
        fprintf(stderr, "Could not initialize parser: ret=%d\n", ret);
        return -1;
    }

    ret = parser_exec(&parser);
    if (ret == 0 && params.gen_header) {
        cart_header_t header;
        cart_header_init(&header, params.outfile);
        cart_header_write(&header, output);
    }

    parser_print_error(&parser, ret);
    parser_finish(&parser);

    fclose(output);
    fclose(input);

    return ret;
}
