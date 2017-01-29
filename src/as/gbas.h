#ifndef __GBAS_H__
#define __GBAS_H__

#include <stdbool.h>
#include <stdio.h>

typedef struct {
    const char *infile;
    const char *outfile;
    bool gen_header;
    FILE *input;
    FILE *output;
} gbas_t;

gbas_t *gbas_init(int argc, char **argv);
void gbas_finish(gbas_t *gbas);

#endif /* __GBAS_H__ */
