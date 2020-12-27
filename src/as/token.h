#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_IDENT = 0x100,
    TOKEN_CHAR,
    TOKEN_CONSTANT,
    TOKEN_STRING_LITERAL,
    TOKEN_END,

    /* registers */
    TOKEN_KW_A,
    TOKEN_KW_F,
    TOKEN_KW_B,
    TOKEN_KW_C,
    TOKEN_KW_D,
    TOKEN_KW_E,
    TOKEN_KW_H,
    TOKEN_KW_L,
    TOKEN_KW_AF,
    TOKEN_KW_BC,
    TOKEN_KW_DE,
    TOKEN_KW_HL,
    TOKEN_KW_SP,

    /* comparison */
    TOKEN_KW_NC,
    TOKEN_KW_NZ,
    TOKEN_KW_Z,

    /* instructions */
    TOKEN_KW_ADC,
    TOKEN_KW_ADD,
    TOKEN_KW_AND,
    TOKEN_KW_CALL,
    TOKEN_KW_CCF,
    TOKEN_KW_CP,
    TOKEN_KW_CPL,
    TOKEN_KW_DAA,
    TOKEN_KW_DEC,
    TOKEN_KW_DI,
    TOKEN_KW_EI,
    TOKEN_KW_HALT,
    TOKEN_KW_INC,
    TOKEN_KW_JP,
    TOKEN_KW_JR,
    TOKEN_KW_LD,
    TOKEN_KW_LDD,
    TOKEN_KW_LDI,
    TOKEN_KW_NOP,
    TOKEN_KW_OR,
    TOKEN_KW_POP,
    TOKEN_KW_PUSH,
    TOKEN_KW_RET,
    TOKEN_KW_RETI,
    TOKEN_KW_RLA,
    TOKEN_KW_RLCA,
    TOKEN_KW_RRA,
    TOKEN_KW_RRCA,
    TOKEN_KW_RST,
    TOKEN_KW_SBC,
    TOKEN_KW_SCF,
    TOKEN_KW_STOP,
    TOKEN_KW_SUB,
    TOKEN_KW_XOR,

    /* extended instructions */
    TOKEN_KW_BIT,
    TOKEN_KW_RES,
    TOKEN_KW_RL,
    TOKEN_KW_RLC,
    TOKEN_KW_RR,
    TOKEN_KW_RRC,
    TOKEN_KW_SET,
    TOKEN_KW_SLA,
    TOKEN_KW_SRA,
    TOKEN_KW_SRL,
    TOKEN_KW_SWAP,

    /* macros */
    TOKEN_KW_ASCII,
    TOKEN_KW_DB,
    TOKEN_KW_ORG,
    TOKEN_KW_FILL,

    TOKEN_KW_INVALID
} token_type_t;

typedef struct {
    token_type_t type;
    char *s;
} token_t;

token_t *token_create(token_type_t type);
token_t *token_create_string(token_type_t type, char *s);
void token_destroy(token_t *t);

#endif /* TOKEN_H */
