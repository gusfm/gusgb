#include <stdio.h>
#include <unistd.h>
#include "as/cart.h"
#include "parser.h"

typedef struct {
    char *infile;
    char *outfile;
    bool gen_header;
} params_t;

static void print_help(char **argv)
{
    fprintf(stderr, "Syntax: %s -i <input file> -o <output file> [options]\n", argv[0]);
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

static int gbas_parse(params_t *params, FILE *input)
{
    parser_t parser;
    int ret;

    FILE *output = fopen(params->outfile, "w+");
    if (output == NULL) {
        fprintf(stderr, "Could not open output file %s\n", params->outfile);
        return -1;
    }

    if (params->gen_header) {
        /* Zero 32k bytes from cartridge */
        for (int i = 0; i < 0x7fff;) {
            int buf = 0;
            ret = fwrite(&buf, sizeof(buf), 1, output);
            if (ret != 1) {
                fprintf(stderr, "Could not zero output file: ret=%d\n", ret);
                return -1;
            }
            i += sizeof(buf);
        }
        /* Write nop; jp $150 to 0x100 address */
        ret = fseek(output, 0x100, SEEK_SET);
        if (ret != 0) {
            fprintf(stderr, "ERROR: could not seek to 0x100\n");
            return -1;
        }
        uint8_t buf[4] = {0x00, 0xc3, 0x50, 0x01};
        ret = fwrite(buf, sizeof(buf), 1, output);
        if (ret != 1) {
            fprintf(stderr, "ERROR: could not write initial jump to $150\n");
            return -1;
        }
        /* Seek to 0x150 so parser can write the output in the correct addr */
        ret = fseek(output, 0x150, SEEK_SET);
        if (ret != 0) {
            fprintf(stderr, "ERROR: could not seek to 0x150\n");
            return -1;
        }
    }

    ret = parser_init(&parser, input, output);
    if (ret != PARSER_OK) {
        fprintf(stderr, "Could not initialize parser: ret=%d\n", ret);
        return -1;
    }

    ret = parser_exec(&parser);
    if (ret == 0 && params->gen_header) {
        cart_header_t header;
        cart_header_init(&header, params->outfile);
        cart_header_write(&header, output);
    }

    parser_print_error(&parser, ret);
    parser_finish(&parser);
    fclose(output);

    return ret;
}

int main(int argc, char **argv)
{
    params_t params;

    if (read_params(argc, argv, &params) != 0) {
        return -1;
    }

    FILE *input = fopen(params.infile, "r");
    if (input == NULL) {
        fprintf(stderr, "Could not open input file %s\n", params.infile);
        return -1;
    }
    int ret = gbas_parse(&params, input);
    fclose(input);

    return ret;
}
