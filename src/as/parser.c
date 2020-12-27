#include "parser.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "op_enc.h"

#define MAX_LABEL_LENGTH 32

#define CHK(f)               \
    do {                     \
        int rv = f;          \
        if (rv != PARSER_OK) \
            return rv;       \
    } while (0)

typedef enum {
    JUMP_JP,
    JUMP_JR,
} jump_type_t;

typedef struct {
    char *str;
    unsigned int addr;
} label_t;

typedef struct {
    jump_type_t type;
    char *label;
    unsigned int addr;
} jump_t;

int parser_init(parser_t *p, FILE *input, FILE *output)
{
    lex_init(&p->l, input);
    op_enc_init(output);
    p->saved_tok = NULL;
    p->label_list = list_create();
    p->jump_list = list_create();
    return 0;
}

void parser_finish(parser_t *p)
{
    while (!list_empty(p->label_list)) {
        label_t *label = list_remove_first(p->label_list);
        free(label->str);
        free(label);
    }
    list_destroy(p->label_list);
    while (!list_empty(p->jump_list)) {
        jump_t *jump = list_remove_first(p->jump_list);
        free(jump->label);
        free(jump);
    }
    list_destroy(p->jump_list);
}

static void parser_label_insert(parser_t *p, char *str, unsigned int addr)
{
    label_t *label = malloc(sizeof(label_t));
    label->str = str;
    label->addr = addr;
    list_insert(p->label_list, label);
}

static unsigned int parser_search_label(parser_t *p, char *str,
                                        unsigned int *jump_addr)
{
    node_t *node = list_get_first(p->label_list);
    while (node != NULL) {
        label_t *label = node->data;
        if (strncmp(str, label->str, MAX_LABEL_LENGTH) == 0) {
            *jump_addr = label->addr;
            return PARSER_OK;
        }
        node = node->forward;
    }
    fprintf(stderr, "error: invalid label: %s\n", str);
    return PARSER_ERR_INVALID_LABEL;
}

static void parser_jump_insert(parser_t *p, char *label, unsigned int addr,
                               jump_type_t type)
{
    jump_t *jump = malloc(sizeof(jump_t));
    jump->type = type;
    jump->label = label;
    jump->addr = addr;
    list_insert(p->jump_list, jump);
}

static int parser_jump_process(parser_t *p)
{
    int ret = PARSER_OK;
    while (!list_empty(p->jump_list)) {
        unsigned int jump_addr;
        jump_t *jump = list_remove_first(p->jump_list);
        int rv = parser_search_label(p, jump->label, &jump_addr);
        if (rv == PARSER_OK) {
            if (jump->type == JUMP_JP) {
                op_enc_jp_addr(jump->addr, jump_addr);
            } else {
                op_enc_jr_addr(jump->addr, jump_addr);
            }
        } else if (ret == PARSER_OK) {
            ret = rv;
        }
        free(jump->label);
        free(jump);
    }
    return ret;
}

static token_t *parser_next_token(parser_t *p)
{
    if (p->saved_tok) {
        token_t *t = p->saved_tok;
        p->saved_tok = NULL;
        return t;
    }
    return lex_next_token(&p->l);
}

static token_t *parser_lookahead_token(parser_t *p)
{
    if (p->saved_tok == NULL)
        p->saved_tok = lex_next_token(&p->l);
    return p->saved_tok;
}

static void parser_lookahead_clear(parser_t *p)
{
    token_destroy(p->saved_tok);
    p->saved_tok = NULL;
}

static token_type_t parser_next_token_type(parser_t *p)
{
    token_type_t type;
    token_t *tok = parser_next_token(p);
    if (tok) {
        type = tok->type;
    } else {
        type = TOKEN_END;
    }
    token_destroy(tok);
    return type;
}

static int parse_reg_8(token_type_t tok_type, reg_t *reg)
{
    switch (tok_type) {
        case TOKEN_KW_A:
            *reg = REG_A;
            return PARSER_OK;
        case TOKEN_KW_F:
            *reg = REG_F;
            return PARSER_OK;
        case TOKEN_KW_B:
            *reg = REG_B;
            return PARSER_OK;
        case TOKEN_KW_C:
            *reg = REG_C;
            return PARSER_OK;
        case TOKEN_KW_D:
            *reg = REG_D;
            return PARSER_OK;
        case TOKEN_KW_E:
            *reg = REG_E;
            return PARSER_OK;
        case TOKEN_KW_H:
            *reg = REG_H;
            return PARSER_OK;
        case TOKEN_KW_L:
            *reg = REG_L;
            return PARSER_OK;
        default:
            return PARSER_ERR_SYNTAX;
    }
}

static int parse_number(parser_t *p, long int *number)
{
    int ret = PARSER_ERR_SYNTAX;
    token_t *tok = parser_next_token(p);
    if (tok->type == TOKEN_CONSTANT) {
        int base = tok->s[1] == 'x' ? 16 : 10;
        *number = strtol(tok->s, NULL, base);
        ret = PARSER_OK;
    }
    token_destroy(tok);
    return ret;
}

static int parse_number_u8(parser_t *p, long int *number)
{
    CHK(parse_number(p, number));
    if (*number & 0xffffff00)
        return PARSER_ERR_NUMBER;
    return PARSER_OK;
}

static int parse_number_s8(parser_t *p, long int *number)
{
    CHK(parse_number(p, number));
    if (*number > 127 || *number < -128)
        return PARSER_ERR_NUMBER_S8;
    return PARSER_OK;
}

static int parse_number_u16(parser_t *p, long int *number)
{
    CHK(parse_number(p, number));
    if (*number & 0xffff0000)
        return PARSER_ERR_NUMBER;
    return PARSER_OK;
}

static int parse_regs_8_hlp(parser_t *p, reg_t *reg)
{
    token_type_t tok_type = parser_next_token_type(p);
    if (parse_reg_8(tok_type, reg) == PARSER_OK) {
        return PARSER_OK;
    } else if (tok_type == '(') {
        tok_type = parser_next_token_type(p);
        if (tok_type != TOKEN_KW_HL)
            return PARSER_ERR_SYNTAX;
        *reg = REG_HL;
        tok_type = parser_next_token_type(p);
        if (tok_type == ')')
            return PARSER_OK;
    }
    return PARSER_ERR_SYNTAX;
}

static int parse_reg_or_n(parser_t *p, void (*freg)(reg_t), void (*fn)(uint8_t))
{
    reg_t reg;
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == TOKEN_CONSTANT) {
        long int n;
        CHK(parse_number_u8(p, &n));
        fn(n);
    } else if (parse_regs_8_hlp(p, &reg) == PARSER_OK) {
        freg(reg);
    } else {
        return PARSER_ERR_SYNTAX;
    }
    return PARSER_OK;
}

static int parse_addr_or_label(parser_t *p, jump_type_t jt, long int *addr)
{
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == TOKEN_IDENT) {
        tok = parser_next_token(p);
        parser_jump_insert(p, tok->s, op_enc_get_pc(), jt);
        tok->s = NULL;
        token_destroy(tok);
        *addr = 0;
    } else {
        if (jt == JUMP_JP)
            CHK(parse_number_u16(p, addr));
        else
            CHK(parse_number_s8(p, addr));
    }
    return PARSER_OK;
}

static int parse_shifts(parser_t *p, void (*f)(reg_t))
{
    reg_t reg;
    CHK(parse_regs_8_hlp(p, &reg));
    f(reg);
    return PARSER_OK;
}

static int parse_adc(parser_t *p)
{
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type != TOKEN_KW_A) {
        return PARSER_ERR_SYNTAX;
    }
    tok_type = parser_next_token_type(p);
    if (tok_type != ',') {
        return PARSER_ERR_SYNTAX;
    }
    return parse_reg_or_n(p, op_enc_adc, op_enc_adc_n);
}

static int parse_add(parser_t *p)
{
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type == TOKEN_KW_A) {
        tok_type = parser_next_token_type(p);
        if (tok_type != ',') {
            return PARSER_ERR_SYNTAX;
        }
        return parse_reg_or_n(p, op_enc_add_a_reg, op_enc_add_a_n);
    } else if (tok_type == TOKEN_KW_HL) {
        tok_type = parser_next_token_type(p);
        if (tok_type != ',') {
            return PARSER_ERR_SYNTAX;
        }
        tok_type = parser_next_token_type(p);
        switch (tok_type) {
            case TOKEN_KW_BC:
                op_enc_add_hl_bc();
                break;
            case TOKEN_KW_DE:
                op_enc_add_hl_de();
                break;
            case TOKEN_KW_HL:
                op_enc_add_hl_hl();
                break;
            case TOKEN_KW_SP:
                op_enc_add_hl_sp();
                break;
            default:
                return PARSER_ERR_SYNTAX;
        }
    } else if (tok_type == TOKEN_KW_SP) {
        long int n;
        tok_type = parser_next_token_type(p);
        if (tok_type != ',') {
            return PARSER_ERR_SYNTAX;
        }
        CHK(parse_number_u8(p, &n));
        op_enc_add_sp_n(n);
    } else {
        return PARSER_ERR_SYNTAX;
    }
    return PARSER_OK;
}

static int parse_number_and_reg(parser_t *p, void (*f)(unsigned int, reg_t))
{
    long int n;
    reg_t reg;
    CHK(parse_number_u8(p, &n));
    if (parser_next_token_type(p) != ',') {
        return PARSER_ERR_SYNTAX;
    }
    if (n > 7) {
        return PARSER_ERR_NUMBER;
    }
    CHK(parse_regs_8_hlp(p, &reg));
    f(n, reg);
    return PARSER_OK;
}

static int parse_call(parser_t *p)
{
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == TOKEN_CONSTANT) {
        long int n;
        CHK(parse_number_u16(p, &n));
        op_enc_call_nn(n);
    } else {
        long int n;
        token_type_t tok_type_cond = parser_next_token_type(p);
        token_type_t tok_type = parser_next_token_type(p);
        if (tok_type != ',') {
            return PARSER_ERR_SYNTAX;
        }
        CHK(parse_number_u16(p, &n));
        switch (tok_type_cond) {
            case TOKEN_KW_C:
                op_enc_call_c_nn(n);
                break;
            case TOKEN_KW_NC:
                op_enc_call_nc_nn(n);
                break;
            case TOKEN_KW_NZ:
                op_enc_call_nz_nn(n);
                break;
            case TOKEN_KW_Z:
                op_enc_call_z_nn(n);
                break;
            default:
                return PARSER_ERR_SYNTAX;
        }
    }
    return PARSER_OK;
}

static int parse_cp(parser_t *p)
{
    return parse_reg_or_n(p, op_enc_cp, op_enc_cp_n);
}

static int parse_db_content(parser_t *p)
{
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == TOKEN_CONSTANT) {
        long int byte;
        CHK(parse_number_u8(p, &byte));
        op_enc_db(byte);
    } else if (tok->type == TOKEN_STRING_LITERAL) {
        op_enc_db_ascii(tok->s);
        parser_lookahead_clear(p);
    } else {
        parser_lookahead_clear(p);
        return PARSER_ERR_SYNTAX;
    }
    return PARSER_OK;
}

static int parse_db(parser_t *p)
{
    CHK(parse_db_content(p));

    while (parser_lookahead_token(p)->type == ',') {
        parser_lookahead_clear(p);
        CHK(parse_db_content(p));
    }

    return PARSER_OK;
}

static int parse_dec(parser_t *p)
{
    token_type_t tok_type = parser_next_token_type(p);
    switch ((int)tok_type) {
        case '(':
            tok_type = parser_next_token_type(p);
            if (tok_type != TOKEN_KW_HL) {
                return PARSER_ERR_SYNTAX;
            }
            tok_type = parser_next_token_type(p);
            if (tok_type != ')') {
                return PARSER_ERR_SYNTAX;
            }
            op_enc_dec_hlp();
            break;
        case TOKEN_KW_A:
            op_enc_dec_a();
            break;
        case TOKEN_KW_B:
            op_enc_dec_b();
            break;
        case TOKEN_KW_BC:
            op_enc_dec_bc();
            break;
        case TOKEN_KW_C:
            op_enc_dec_c();
            break;
        case TOKEN_KW_D:
            op_enc_dec_d();
            break;
        case TOKEN_KW_DE:
            op_enc_dec_de();
            break;
        case TOKEN_KW_E:
            op_enc_dec_e();
            break;
        case TOKEN_KW_H:
            op_enc_dec_h();
            break;
        case TOKEN_KW_HL:
            op_enc_dec_hl();
            break;
        case TOKEN_KW_L:
            op_enc_dec_l();
            break;
        case TOKEN_KW_SP:
            op_enc_dec_sp();
            break;
        default:
            return PARSER_ERR_SYNTAX;
    }
    return PARSER_OK;
}

static int parse_inc(parser_t *p)
{
    token_type_t tok_type = parser_next_token_type(p);
    switch ((int)tok_type) {
        case '(':
            tok_type = parser_next_token_type(p);
            if (tok_type != TOKEN_KW_HL) {
                return PARSER_ERR_SYNTAX;
            }
            tok_type = parser_next_token_type(p);
            if (tok_type != ')') {
                return PARSER_ERR_SYNTAX;
            }
            op_enc_inc_hlp();
            break;
        case TOKEN_KW_A:
            op_enc_inc_a();
            break;
        case TOKEN_KW_B:
            op_enc_inc_b();
            break;
        case TOKEN_KW_BC:
            op_enc_inc_bc();
            break;
        case TOKEN_KW_C:
            op_enc_inc_c();
            break;
        case TOKEN_KW_D:
            op_enc_inc_d();
            break;
        case TOKEN_KW_DE:
            op_enc_inc_de();
            break;
        case TOKEN_KW_E:
            op_enc_inc_e();
            break;
        case TOKEN_KW_H:
            op_enc_inc_h();
            break;
        case TOKEN_KW_HL:
            op_enc_inc_hl();
            break;
        case TOKEN_KW_L:
            op_enc_inc_l();
            break;
        case TOKEN_KW_SP:
            op_enc_inc_sp();
            break;
        default:
            return PARSER_ERR_SYNTAX;
    }
    return PARSER_OK;
}

static int parse_jp(parser_t *p)
{
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == TOKEN_CONSTANT || tok->type == TOKEN_IDENT) {
        long int n;
        CHK(parse_addr_or_label(p, JUMP_JP, &n));
        op_enc_jp_nn(n);
    } else {
        token_type_t tok_type = parser_next_token_type(p);
        if (tok_type == '(') {
            tok_type = parser_next_token_type(p);
            if (tok_type != TOKEN_KW_HL) {
                return PARSER_ERR_SYNTAX;
            }
            tok_type = parser_next_token_type(p);
            if (tok_type != ')') {
                return PARSER_ERR_SYNTAX;
            }
            op_enc_jp_hl();
        } else {
            long int n;
            token_type_t tok_type_cond = tok_type;
            tok_type = parser_next_token_type(p);
            if (tok_type != ',') {
                return PARSER_ERR_SYNTAX;
            }
            CHK(parse_addr_or_label(p, JUMP_JP, &n));
            switch (tok_type_cond) {
                case TOKEN_KW_C:
                    op_enc_jp_c_nn(n);
                    break;
                case TOKEN_KW_NC:
                    op_enc_jp_nc_nn(n);
                    break;
                case TOKEN_KW_NZ:
                    op_enc_jp_nz_nn(n);
                    break;
                case TOKEN_KW_Z:
                    op_enc_jp_z_nn(n);
                    break;
                default:
                    return PARSER_ERR_SYNTAX;
            }
        }
    }
    return PARSER_OK;
}

static int parse_jr(parser_t *p)
{
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == TOKEN_CONSTANT || tok->type == TOKEN_IDENT) {
        long int n;
        CHK(parse_addr_or_label(p, JUMP_JR, &n));
        op_enc_jr_n(n);
    } else {
        long int n;
        token_type_t tok_type_cond = parser_next_token_type(p);
        token_type_t tok_type = parser_next_token_type(p);
        if (tok_type != ',') {
            return PARSER_ERR_SYNTAX;
        }
        CHK(parse_addr_or_label(p, JUMP_JR, &n));
        switch (tok_type_cond) {
            case TOKEN_KW_C:
                op_enc_jr_c_n(n);
                break;
            case TOKEN_KW_NC:
                op_enc_jr_nc_n(n);
                break;
            case TOKEN_KW_NZ:
                op_enc_jr_nz_n(n);
                break;
            case TOKEN_KW_Z:
                op_enc_jr_z_n(n);
                break;
            default:
                return PARSER_ERR_SYNTAX;
        }
    }
    return PARSER_OK;
}

static int parse_ld_a(parser_t *p)
{
    reg_t reg;
    long int n;
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type != ',') {
        return PARSER_ERR_SYNTAX;
    }
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == '(') {
        parser_lookahead_clear(p);
        tok = parser_lookahead_token(p);
        if (tok->type == TOKEN_KW_BC) {
            parser_lookahead_clear(p);
            op_enc_ld_a_bcp();
        } else if (tok->type == TOKEN_KW_DE) {
            parser_lookahead_clear(p);
            op_enc_ld_a_dep();
        } else if (tok->type == TOKEN_KW_HL) {
            parser_lookahead_clear(p);
            op_enc_ld_a_reg(REG_HL);
        } else {
            CHK(parse_number_u16(p, &n));
            tok = parser_lookahead_token(p);
            if (tok->type == '+') {
                parser_lookahead_clear(p);
                if (n != 0xff00) {
                    return PARSER_ERR_NUMBER_FF00;
                }
                tok = parser_lookahead_token(p);
                if (tok->type == TOKEN_KW_C) {
                    parser_lookahead_clear(p);
                    op_enc_ld_a_cp();
                } else {
                    CHK(parse_number_u8(p, &n));
                    op_enc_ldh_a_n(n);
                }
            } else {
                op_enc_ld_a_nnp(n);
            }
        }
        if (parser_next_token_type(p) != ')') {
            return PARSER_ERR_SYNTAX;
        }
    } else if (parse_reg_8(tok->type, &reg) == PARSER_OK) {
        parser_lookahead_clear(p);
        op_enc_ld_a_reg(reg);
    } else {
        CHK(parse_number_u8(p, &n));
        op_enc_ld_a_n(n);
    }
    return PARSER_OK;
}

static int parse_ld_reg8(parser_t *p, void (*freg)(reg_t), void (*fn)(uint8_t))
{
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type != ',') {
        return PARSER_ERR_SYNTAX;
    }
    return parse_reg_or_n(p, freg, fn);
}

static int parse_ld_reg16(parser_t *p, void (*f)(uint16_t))
{
    long int n;
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type != ',') {
        return PARSER_ERR_SYNTAX;
    }
    CHK(parse_number_u16(p, &n));
    f(n);
    return PARSER_OK;
}

static int parse_ld_hl(parser_t *p)
{
    long int n;
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type != ',') {
        return PARSER_ERR_SYNTAX;
    }
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == TOKEN_KW_SP) {
        parser_lookahead_clear(p);
        tok = parser_lookahead_token(p);
        if (tok->type != '+') {
            return PARSER_ERR_SYNTAX;
        }
        parser_lookahead_clear(p);
        CHK(parse_number_s8(p, &n));
        op_enc_ldhl_sp_n(n);
    } else {
        CHK(parse_number_u16(p, &n));
        op_enc_ld_hl_nn(n);
    }
    return PARSER_OK;
}

static int parse_ld_sp(parser_t *p)
{
    long int n;
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type != ',') {
        return PARSER_ERR_SYNTAX;
    }
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == TOKEN_KW_HL) {
        parser_lookahead_clear(p);
        op_enc_ld_sp_hl();
    } else {
        CHK(parse_number_u16(p, &n));
        op_enc_ld_sp_nn(n);
    }
    return PARSER_OK;
}

static int parse_ld_store_bc(parser_t *p)
{
    parser_lookahead_clear(p);
    if (parser_next_token_type(p) != ')') {
        return PARSER_ERR_SYNTAX;
    }
    if (parser_next_token_type(p) != ',') {
        return PARSER_ERR_SYNTAX;
    }
    if (parser_next_token_type(p) != TOKEN_KW_A) {
        return PARSER_ERR_SYNTAX;
    }
    op_enc_ld_bcp_a();
    return PARSER_OK;
}

static int parse_ld_store_de(parser_t *p)
{
    parser_lookahead_clear(p);
    if (parser_next_token_type(p) != ')') {
        return PARSER_ERR_SYNTAX;
    }
    if (parser_next_token_type(p) != ',') {
        return PARSER_ERR_SYNTAX;
    }
    if (parser_next_token_type(p) != TOKEN_KW_A) {
        return PARSER_ERR_SYNTAX;
    }
    op_enc_ld_dep_a();
    return PARSER_OK;
}

static int parse_ld_store_hl(parser_t *p)
{
    reg_t reg;
    parser_lookahead_clear(p);
    if (parser_next_token_type(p) != ')') {
        return PARSER_ERR_SYNTAX;
    }
    if (parser_next_token_type(p) != ',') {
        return PARSER_ERR_SYNTAX;
    }
    token_t *tok = parser_lookahead_token(p);
    if (parse_reg_8(tok->type, &reg) == PARSER_OK) {
        parser_lookahead_clear(p);
        op_enc_ld_hlp_reg(reg);
    } else {
        long int n;
        CHK(parse_number_u8(p, &n));
        op_enc_ld_hlp_n(n);
    }
    return PARSER_OK;
}

static int parse_ld_store_nn(parser_t *p)
{
    long int n;
    CHK(parse_number_u16(p, &n));
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == '+') {
        parser_lookahead_clear(p);
        if (n != 0xff00) {
            return PARSER_ERR_NUMBER_FF00;
        }
        tok = parser_lookahead_token(p);
        if (tok->type == TOKEN_KW_C) {
            parser_lookahead_clear(p);
            op_enc_ld_cp_a();
        } else {
            CHK(parse_number_u8(p, &n));
            op_enc_ldh_n_a(n);
        }
        if (parser_next_token_type(p) != ')') {
            return PARSER_ERR_SYNTAX;
        }
        if (parser_next_token_type(p) != ',') {
            return PARSER_ERR_SYNTAX;
        }
        if (parser_next_token_type(p) != TOKEN_KW_A) {
            return PARSER_ERR_SYNTAX;
        }
    } else {
        if (parser_next_token_type(p) != ')') {
            return PARSER_ERR_SYNTAX;
        }
        if (parser_next_token_type(p) != ',') {
            return PARSER_ERR_SYNTAX;
        }
        token_type_t tok_type = parser_next_token_type(p);
        if (tok_type == TOKEN_KW_A) {
            op_enc_ld_nnp_a(n);
        } else if (tok_type == TOKEN_KW_SP) {
            op_enc_ld_nnp_sp(n);
        } else {
            return PARSER_ERR_SYNTAX;
        }
    }
    return PARSER_OK;
}

static int parse_ld_store(parser_t *p)
{
    token_t *tok = parser_lookahead_token(p);
    if (tok->type == TOKEN_KW_BC) {
        return parse_ld_store_bc(p);
    } else if (tok->type == TOKEN_KW_DE) {
        return parse_ld_store_de(p);
    } else if (tok->type == TOKEN_KW_HL) {
        return parse_ld_store_hl(p);
    } else {
        return parse_ld_store_nn(p);
    }
}

static int parse_ld(parser_t *p)
{
    token_type_t tok_type = parser_next_token_type(p);
    switch ((int)tok_type) {
        case TOKEN_KW_A:
            return parse_ld_a(p);
        case TOKEN_KW_B:
            return parse_ld_reg8(p, op_enc_ld_b_reg, op_enc_ld_b_n);
        case TOKEN_KW_C:
            return parse_ld_reg8(p, op_enc_ld_c_reg, op_enc_ld_c_n);
        case TOKEN_KW_D:
            return parse_ld_reg8(p, op_enc_ld_d_reg, op_enc_ld_d_n);
        case TOKEN_KW_E:
            return parse_ld_reg8(p, op_enc_ld_e_reg, op_enc_ld_e_n);
        case TOKEN_KW_H:
            return parse_ld_reg8(p, op_enc_ld_h_reg, op_enc_ld_h_n);
        case TOKEN_KW_L:
            return parse_ld_reg8(p, op_enc_ld_l_reg, op_enc_ld_l_n);
        case TOKEN_KW_BC:
            return parse_ld_reg16(p, op_enc_ld_bc_nn);
        case TOKEN_KW_DE:
            return parse_ld_reg16(p, op_enc_ld_de_nn);
        case TOKEN_KW_HL:
            return parse_ld_hl(p);
        case TOKEN_KW_SP:
            return parse_ld_sp(p);
        case '(':
            return parse_ld_store(p);
        default:
            return PARSER_ERR_SYNTAX;
    }
}

static int parse_ldi_ldd(parser_t *p, void (*fload)(void), void (*fstore)(void))
{
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type == TOKEN_KW_A) {
        if (parser_next_token_type(p) != ',') {
            return PARSER_ERR_SYNTAX;
        }
        if (parser_next_token_type(p) != '(') {
            return PARSER_ERR_SYNTAX;
        }
        if (parser_next_token_type(p) != TOKEN_KW_HL) {
            return PARSER_ERR_SYNTAX;
        }
        if (parser_next_token_type(p) != ')') {
            return PARSER_ERR_SYNTAX;
        }
        fload();
    } else if (tok_type == '(') {
        if (parser_next_token_type(p) != TOKEN_KW_HL) {
            return PARSER_ERR_SYNTAX;
        }
        if (parser_next_token_type(p) != ')') {
            return PARSER_ERR_SYNTAX;
        }
        if (parser_next_token_type(p) != ',') {
            return PARSER_ERR_SYNTAX;
        }
        if (parser_next_token_type(p) != TOKEN_KW_A) {
            return PARSER_ERR_SYNTAX;
        }
        fstore();
    } else {
        return PARSER_ERR_SYNTAX;
    }
    return PARSER_OK;
}

static int parse_pop(parser_t *p)
{
    token_type_t tok_type = parser_next_token_type(p);
    switch (tok_type) {
        case TOKEN_KW_AF:
            op_enc_pop_af();
            break;
        case TOKEN_KW_BC:
            op_enc_pop_bc();
            break;
        case TOKEN_KW_DE:
            op_enc_pop_de();
            break;
        case TOKEN_KW_HL:
            op_enc_pop_hl();
            break;
        default:
            return PARSER_ERR_SYNTAX;
    }
    return PARSER_OK;
}

static int parse_push(parser_t *p)
{
    token_type_t tok_type = parser_next_token_type(p);
    switch (tok_type) {
        case TOKEN_KW_AF:
            op_enc_push_af();
            break;
        case TOKEN_KW_BC:
            op_enc_push_bc();
            break;
        case TOKEN_KW_DE:
            op_enc_push_de();
            break;
        case TOKEN_KW_HL:
            op_enc_push_hl();
            break;
        default:
            return PARSER_ERR_SYNTAX;
    }
    return PARSER_OK;
}

static int parse_ret(parser_t *p)
{
    token_t *tok = parser_lookahead_token(p);
    switch (tok->type) {
        case TOKEN_KW_C:
            op_enc_ret_c();
            parser_lookahead_clear(p);
            break;
        case TOKEN_KW_NC:
            op_enc_ret_nc();
            parser_lookahead_clear(p);
            break;
        case TOKEN_KW_NZ:
            op_enc_ret_nz();
            parser_lookahead_clear(p);
            break;
        case TOKEN_KW_Z:
            op_enc_ret_z();
            parser_lookahead_clear(p);
            break;
        default:
            op_enc_ret();
            break;
    }
    return PARSER_OK;
}

static int parse_sbc(parser_t *p)
{
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type != TOKEN_KW_A) {
        return PARSER_ERR_SYNTAX;
    }
    tok_type = parser_next_token_type(p);
    if (tok_type != ',') {
        return PARSER_ERR_SYNTAX;
    }
    return parse_reg_or_n(p, op_enc_sbc, op_enc_sbc_n);
}

static int parse_rst(parser_t *p)
{
    long int n;
    CHK(parse_number_u8(p, &n));
    if (n != 0x0 && n != 0x8 && n != 0x10 && n != 0x18 && n != 0x20 &&
        n != 0x28 && n != 0x30 && n != 0x38) {
        return PARSER_ERR_SYNTAX;
    }
    op_enc_rst(n);
    return PARSER_OK;
}

static int parse_cmd(parser_t *p, token_t *tok)
{
    token_type_t tok_type = tok->type;
    token_destroy(tok);
    switch (tok_type) {
        case TOKEN_KW_ADC:
            return parse_adc(p);
        case TOKEN_KW_ADD:
            return parse_add(p);
        case TOKEN_KW_AND:
            return parse_reg_or_n(p, op_enc_and, op_enc_and_n);
        case TOKEN_KW_BIT:
            return parse_number_and_reg(p, op_enc_bit);
        case TOKEN_KW_CALL:
            return parse_call(p);
        case TOKEN_KW_CCF:
            op_enc_ccf();
            return PARSER_OK;
        case TOKEN_KW_CP:
            return parse_cp(p);
        case TOKEN_KW_CPL:
            op_enc_cpl();
            return PARSER_OK;
        case TOKEN_KW_DAA:
            op_enc_daa();
            return PARSER_OK;
        case TOKEN_KW_DB:
            return parse_db(p);
        case TOKEN_KW_DEC:
            return parse_dec(p);
        case TOKEN_KW_DI:
            op_enc_di();
            return PARSER_OK;
        case TOKEN_KW_EI:
            op_enc_ei();
            return PARSER_OK;
        case TOKEN_KW_HALT:
            op_enc_halt();
            return PARSER_OK;
        case TOKEN_KW_INC:
            return parse_inc(p);
        case TOKEN_KW_JP:
            return parse_jp(p);
        case TOKEN_KW_JR:
            return parse_jr(p);
        case TOKEN_KW_LD:
            return parse_ld(p);
        case TOKEN_KW_LDD:
            return parse_ldi_ldd(p, op_enc_ldd_a_hlp, op_enc_ldd_hlp_a);
        case TOKEN_KW_LDI:
            return parse_ldi_ldd(p, op_enc_ldi_a_hlp, op_enc_ldi_hlp_a);
        case TOKEN_KW_NOP:
            op_enc_nop();
            return PARSER_OK;
        case TOKEN_KW_OR:
            return parse_reg_or_n(p, op_enc_or, op_enc_or_n);
        case TOKEN_KW_POP:
            return parse_pop(p);
        case TOKEN_KW_PUSH:
            return parse_push(p);
        case TOKEN_KW_RES:
            return parse_number_and_reg(p, op_enc_res);
        case TOKEN_KW_RET:
            return parse_ret(p);
        case TOKEN_KW_RETI:
            op_enc_reti();
            return PARSER_OK;
        case TOKEN_KW_RL:
            return parse_shifts(p, op_enc_rl);
        case TOKEN_KW_RLC:
            return parse_shifts(p, op_enc_rlc);
        case TOKEN_KW_RLA:
            op_enc_rla();
            return PARSER_OK;
        case TOKEN_KW_RLCA:
            op_enc_rlca();
            return PARSER_OK;
        case TOKEN_KW_RR:
            return parse_shifts(p, op_enc_rr);
        case TOKEN_KW_RRA:
            op_enc_rra();
            return PARSER_OK;
        case TOKEN_KW_RRC:
            return parse_shifts(p, op_enc_rrc);
        case TOKEN_KW_RRCA:
            op_enc_rrca();
            return PARSER_OK;
        case TOKEN_KW_RST:
            return parse_rst(p);
        case TOKEN_KW_SBC:
            return parse_sbc(p);
        case TOKEN_KW_SCF:
            op_enc_scf();
            return PARSER_OK;
        case TOKEN_KW_SET:
            return parse_number_and_reg(p, op_enc_set);
        case TOKEN_KW_SLA:
            return parse_shifts(p, op_enc_sla);
        case TOKEN_KW_SRA:
            return parse_shifts(p, op_enc_sra);
        case TOKEN_KW_SRL:
            return parse_shifts(p, op_enc_srl);
        case TOKEN_KW_STOP:
            op_enc_stop();
            return PARSER_OK;
        case TOKEN_KW_SUB:
            return parse_reg_or_n(p, op_enc_sub, op_enc_sub_n);
        case TOKEN_KW_SWAP:
            return parse_shifts(p, op_enc_swap);
        case TOKEN_KW_XOR:
            return parse_reg_or_n(p, op_enc_xor, op_enc_xor_n);
        case TOKEN_END:
            return PARSER_OK;
        default:
            return PARSER_ERR_CMD;
    }
}

static int parse_label(parser_t *p, token_t *tok)
{
    token_type_t tok_type = parser_next_token_type(p);
    if (tok_type != ':') {
        return PARSER_ERR_SYNTAX;
    }
    parser_label_insert(p, tok->s, op_enc_get_pc());
    tok->s = NULL;
    return PARSER_OK;
}

static int parse_next(parser_t *p, token_t *tok)
{
    if (tok->type == TOKEN_IDENT) {
        int ret = parse_label(p, tok);
        token_destroy(tok);
        return ret;
    } else {
        return parse_cmd(p, tok);
    }
}

int parser_exec(parser_t *p)
{
    token_t *tok;
    while ((tok = parser_next_token(p)) != NULL) {
        int ret;
        if ((ret = parse_next(p, tok)) != PARSER_OK) {
            return ret;
        }
        lex_checkpoint(&p->l);
    }
    return parser_jump_process(p);
}

static const char *parser_strerror(int error)
{
    switch (error) {
        case PARSER_OK:
            return "";
        case PARSER_ERR_CMD:
            return "invalid command";
        case PARSER_ERR_SYNTAX:
            return "invalid syntax";
        case PARSER_ERR_NUMBER:
            return "maximum number size exceeded";
        case PARSER_ERR_NUMBER_S8:
            return "maximum number size exceeded for signed 8 bit number";
        case PARSER_ERR_NUMBER_FF00:
            return "instruction accepts only 0xff00";
        default:
            return "unknown error";
    }
}

void parser_print_error(parser_t *p, int error)
{
    if (error != PARSER_OK && error != PARSER_ERR_INVALID_LABEL) {
        lex_error(&p->l, parser_strerror(error));
    }
}
