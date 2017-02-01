#ifndef __GBAS_H__
#define __GBAS_H__

#include <stdbool.h>
#include <stdio.h>
#include "list.h"

typedef struct {
    const char *infile;
    const char *outfile;
    bool gen_header;
    FILE *input;
    FILE *output;
    list_t *label_list;
} gbas_t;

gbas_t *gbas_init(int argc, char **argv);
void gbas_finish(gbas_t *gbas);
void gbas_label_insert(gbas_t *gbas, char *str, unsigned int addr);
unsigned int gbas_label_get_addr(gbas_t *gbas, char *label);

#endif /* __GBAS_H__ */
