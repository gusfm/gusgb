#include "lex.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "token.h"

#define NUM_KEYWORDS (sizeof(keywords) / sizeof(keywords[0]))

/* clang-format off */

static struct keyword {
    const char *str;
    token_type_t value;
} keywords[] = {
    { "a", TOKEN_KW_A },
    { "adc", TOKEN_KW_ADC },
    { "add", TOKEN_KW_ADD },
    { "af", TOKEN_KW_AF },
    { "and", TOKEN_KW_AND },
    { "b", TOKEN_KW_B },
    { "bc", TOKEN_KW_BC },
    { "bit", TOKEN_KW_BIT },
    { "c", TOKEN_KW_C },
    { "call", TOKEN_KW_CALL },
    { "ccf", TOKEN_KW_CCF },
    { "cp", TOKEN_KW_CP },
    { "cpl", TOKEN_KW_CPL },
    { "d", TOKEN_KW_D },
    { "daa", TOKEN_KW_DAA },
    { "db", TOKEN_KW_DB },
    { "de", TOKEN_KW_DE },
    { "dec", TOKEN_KW_DEC },
    { "di", TOKEN_KW_DI },
    { "e", TOKEN_KW_E },
    { "ei", TOKEN_KW_EI },
    { "f", TOKEN_KW_F },
    { "h", TOKEN_KW_H },
    { "halt", TOKEN_KW_HALT },
    { "hl", TOKEN_KW_HL },
    { "inc", TOKEN_KW_INC },
    { "jp", TOKEN_KW_JP },
    { "jr", TOKEN_KW_JR },
    { "l", TOKEN_KW_L },
    { "ld", TOKEN_KW_LD },
    { "ldd", TOKEN_KW_LDD },
    { "ldi", TOKEN_KW_LDI },
    { "nc", TOKEN_KW_NC },
    { "nop", TOKEN_KW_NOP },
    { "nz", TOKEN_KW_NZ },
    { "or", TOKEN_KW_OR },
    { "pop", TOKEN_KW_POP },
    { "push", TOKEN_KW_PUSH },
    { "res", TOKEN_KW_RES },
    { "ret", TOKEN_KW_RET },
    { "reti", TOKEN_KW_RETI },
    { "rl", TOKEN_KW_RL },
    { "rla", TOKEN_KW_RLA },
    { "rlc", TOKEN_KW_RLC },
    { "rlca", TOKEN_KW_RLCA },
    { "rr", TOKEN_KW_RR },
    { "rra", TOKEN_KW_RRA },
    { "rrc", TOKEN_KW_RRC },
    { "rrca", TOKEN_KW_RRCA },
    { "rst", TOKEN_KW_RST },
    { "sbc", TOKEN_KW_SBC },
    { "scf", TOKEN_KW_SCF },
    { "set", TOKEN_KW_SET },
    { "sla", TOKEN_KW_SLA },
    { "sp", TOKEN_KW_SP },
    { "sra", TOKEN_KW_SRA },
    { "srl", TOKEN_KW_SRL },
    { "stop", TOKEN_KW_STOP },
    { "sub", TOKEN_KW_SUB },
    { "swap", TOKEN_KW_SWAP },
    { "xor", TOKEN_KW_XOR },
    { "z", TOKEN_KW_Z },
};

/* clang-format on */

static int lex_readc(lex_t *l)
{
    int c = fgetc(l->input);
    if (c == '\n') {
        l->line++;
        l->col = 1;
    } else {
        l->col++;
    }
    return c;
}

static int lex_ungetc(lex_t *l, int c)
{
    l->col--;
    return ungetc(c, l->input);
}

static void lex_skip_space(lex_t *l)
{
    int c;
    while ((c = lex_readc(l)) != EOF) {
        if (isblank(c)) {
            continue;
        }
        lex_ungetc(l, c);
        return;
    }
}

static int keyword_compare(const void *k1, const void *k2)
{
    struct keyword *kw1 = (struct keyword *)k1;
    struct keyword *kw2 = (struct keyword *)k2;
    return strcmp(kw1->str, kw2->str);
}

static token_type_t get_token_type(char *s)
{
    struct keyword kw, *res;
    kw.str = s;
    res = bsearch(&kw, keywords, NUM_KEYWORDS, sizeof(kw), keyword_compare);
    if (res) {
        return res->value;
    }
    return TOKEN_IDENT;
}

static token_t *read_keyword(lex_t *l, int c)
{
    str_t *str = str_create();
    str_append(str, (char)c);
    for (;;) {
        token_type_t tok_type;
        char *tok_str;
        c = lex_readc(l);
        if (isalnum(c) || c == '_') {
            str_append(str, (char)c);
            continue;
        }
        lex_ungetc(l, c);
        str_append(str, '\0');
        tok_str = str_destroy(str);
        tok_type = get_token_type(tok_str);
        if (tok_type == TOKEN_IDENT) {
            return token_create_string(tok_type, tok_str);
        }
        free(tok_str);
        return token_create(tok_type);
    }
}

static token_t *read_number(lex_t *l, int c)
{
    str_t *str = str_create();
    str_append(str, (char)c);
    for (;;) {
        c = lex_readc(l);
        if (isxdigit(c) || c == 'x') {
            str_append(str, (char)c);
            continue;
        }
        lex_ungetc(l, c);
        str_append(str, '\0');
        return token_create_string(TOKEN_CONSTANT, str_destroy(str));
    }
}

static token_t *read_string(lex_t *l, char c)
{
    str_t *str = str_create();
    for (;;) {
        c = lex_readc(l);
        if (c != '"') {
            str_append(str, c);
            continue;
        }
        str_append(str, '\0');
        return token_create_string(TOKEN_STRING_LITERAL, str_destroy(str));
    }
}

token_t *lex_next_token(lex_t *l)
{
    int c;
    lex_skip_space(l);
    l->last_tok_line = l->line;
    l->last_tok_col = l->col;
    c = lex_readc(l);
    switch (c) {
        case '"':
            return read_string(l, c);
        case '(':
        case ')':
        case '+':
        case ',':
        case ':':
        case '.':
            return token_create(c);
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            return read_number(l, c);
        case '\0':
        case '\n':
        case '\r':
            return token_create(TOKEN_END);
        case EOF:
            return NULL;
        default:
            return read_keyword(l, c);
    }
}

int lex_init(lex_t *l, FILE *input)
{
    l->input = input;
    l->line = 1;
    l->col = 1;
    l->checkpoint = 0;
    return 0;
}

void lex_checkpoint(lex_t *l)
{
    l->checkpoint = ftell(l->input);
}

void lex_error(lex_t *l, const char *msg)
{
    char str[100];
    fseek(l->input, l->checkpoint, SEEK_SET);
    if (fgets(str, sizeof(str), l->input) == NULL) {
        fprintf(stderr, "%s:%d: error: fgets\n", __func__, __LINE__);
        return;
    }
    fprintf(stderr, "%d:%d: error: %s\n%s\n", l->last_tok_line, l->last_tok_col,
            msg, str);
    for (int i = 1; i < l->last_tok_col; ++i) {
        fputc(' ', stderr);
    }
    fprintf(stderr, "^\n");
}
