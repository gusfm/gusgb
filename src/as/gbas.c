#include "gbas.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LABEL_LENGTH 32
extern unsigned int linenum;

typedef struct {
    char *str;
    unsigned int addr;
} label_t;

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
    gbas->label_list = list_create();
    return gbas;
}

void gbas_finish(gbas_t *gbas)
{
    while (!list_empty(gbas->label_list)) {
        label_t *label = list_remove_first(gbas->label_list);
        free(label->str);
        free(label);
    }
    list_destroy(gbas->label_list);
    fclose(gbas->input);
    fclose(gbas->output);
    free(gbas);
}

void gbas_label_insert(gbas_t *gbas, char *str, unsigned int addr)
{
    label_t *label = malloc(sizeof(label_t));
    label->str = str;
    label->addr = addr;
    list_insert(gbas->label_list, label);
}

unsigned int gbas_label_get_addr(gbas_t *gbas, char *str)
{
    node_t *node = list_get_first(gbas->label_list);
    while (node != NULL) {
        label_t *label = node->data;
        if (strncmp(str, label->str, MAX_LABEL_LENGTH) == 0) {
            return label->addr;
        }
        node = node->forward;
    }
    fprintf(stderr, "ERROR:%u: invalid label: %s\n", linenum, str);
    exit(EXIT_FAILURE);
}
