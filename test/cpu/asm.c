#include "asm.h"
#include <stdio.h>
#include <string.h>
#include "as/parser.h"
#include "ut.h"

int asm_test(const char *src, uint32_t opcode)
{
    parser_t p;
    uint32_t buf[2];
    memset(buf, 0, sizeof(buf));
    FILE *in = fmemopen((void *)src, strlen(src) + 1, "r");
    FILE *out = fmemopen(buf, sizeof(buf), "wb");
    ASSERT(in != NULL);
    ASSERT(out != NULL);
    ASSERT(parser_init(&p, in, out) == 0);
    int ret = parser_exec(&p);
    parser_print_error(&p, ret);
    parser_finish(&p);
    fclose(in);
    fclose(out);
    ASSERT_EQ(opcode, buf[0]);
    return ret;
}

int asm_to_opcode(const char *src, void *buf, size_t buf_size)
{
    parser_t p;
    FILE *in = fmemopen((void *)src, strlen(src) + 1, "r");
    FILE *out = fmemopen(buf, buf_size, "wb");
    ASSERT(in != NULL);
    ASSERT(out != NULL);
    parser_init(&p, in, out);
    int ret = parser_exec(&p);
    parser_print_error(&p, ret);
    parser_finish(&p);
    fclose(in);
    fclose(out);
    return ret;
}
