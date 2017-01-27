%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "as/opcodes.h"

/* stuff from flex that bison needs to know about */
extern int yylex();
extern FILE *yyin;
FILE *output;
extern unsigned int linenum;
extern const char *yytext;
void yyerror(const char *s);

register_e last_reg;
%}

/* */
%union {
    unsigned int num;
    char *sval;
}

%token <num> NUMBER
%token <sval> COMMENT
%token OPEN_PAR
%token CLOSE_PAR
/* Registers */
%token A
%token F
%token B
%token C
%token D
%token E
%token H
%token L
%token AF
%token BC
%token DE
%token HL
%token SP

/* Commands */
%token ADC
%token ADD
%token AND
%token CALL
%token CCF
%token CP
%token CPL
%token DAA
%token DATA
%token DEC
%token DI
%token EI
%token HALT
%token INC
%token JP
%token JR
%token LD
%token LDD
%token LDH
%token LDI
%token NOP
%token OR
%token POP
%token PUSH
%token RET
%token RETI
%token RLA
%token RLCA
%token RRA
%token RRCA
%token RST
%token SBC
%token SCF
%token STOP
%token SUB
%token XOR
/* CB */
%token BIT;
%token RES;
%token RL;
%token RLC;
%token RR;
%token RRC;
%token SET;
%token SLA;
%token SRA;
%token SRL;
%token SWAP;

/* Sub commands. */
%token NC
%token NZ
%token Z

%%

commands:
        | commands command
        ;

regs_8:
        A             { last_reg = REG_A; }
      | B             { last_reg = REG_B; }
      | C             { last_reg = REG_C; }
      | D             { last_reg = REG_D; }
      | E             { last_reg = REG_E; }
      | H             { last_reg = REG_H; }
      | L             { last_reg = REG_L; }
      | '(' HL ')'    { last_reg = REG_HL; }
      ;

adc_cmd:
          NUMBER                { adc_n(output, $1); }
        | regs_8                { adc(output, last_reg); }
        ;

add_cmd:
          A                     { add_a_a(output); }
        | A ',' '(' HL ')'      { add_a_hlp(output); }
        | A ',' B               { add_a_b(output); }
        | A ',' C               { add_a_c(output); }
        | A ',' D               { add_a_d(output); }
        | A ',' E               { add_a_e(output); }
        | A ',' H               { add_a_h(output); }
        | A ',' L               { add_a_l(output); }
        | A ',' NUMBER          { add_a_n(output, $3); }
        | HL ',' BC             { add_hl_bc(output); }
        | HL ',' DE             { add_hl_de(output); }
        | HL ',' HL             { add_hl_hl(output); }
        | HL ',' SP             { add_hl_sp(output); }
        | SP ',' NUMBER         { add_sp_n(output, $3); }
        ;

and_cmd:
          NUMBER                { and_n(output, $1); }
        | regs_8                { andf(output, last_reg); }
        ;

call_cmd:
          C ',' NUMBER         { call_c_nn(output, $3); }
        | NC ',' NUMBER        { call_nc_nn(output, $3); }
        | NUMBER               { call_nn(output, $1); }
        | NZ ',' NUMBER        { call_nz_nn(output, $3); }
        | Z ',' NUMBER         { call_z_nn(output, $3); }
        ;

cp_cmd:
          NUMBER                 { cp_n(output, $1); }
        | regs_8                 { cp(output, last_reg); }
        ;

dec_cmd:
          '(' HL ')'            { dec_hlp(output); }
        | A                     { dec_a(output); }
        | B                     { dec_b(output); }
        | BC                    { dec_bc(output); }
        | C                     { dec_c(output); }
        | D                     { dec_d(output); }
        | DE                    { dec_de(output); }
        | E                     { dec_e(output); }
        | H                     { dec_h(output); }
        | HL                    { dec_hl(output); }
        | L                     { dec_l(output); }
        | SP                    { dec_sp(output); }
        ;

inc_cmd:
          '(' HL ')'            { inc_hlp(output); }
        | A                     { inc_a(output); }
        | B                     { inc_b(output); }
        | BC                    { inc_bc(output); }
        | C                     { inc_c(output); }
        | D                     { inc_d(output); }
        | DE                    { inc_de(output); }
        | E                     { inc_e(output); }
        | H                     { inc_h(output); }
        | HL                    { inc_hl(output); }
        | L                     { inc_l(output); }
        | SP                    { inc_sp(output); }
        ;

jp_cmd:
          C ',' NUMBER           { jp_c_nn(output, $3); }
        | HL                     { jp_hl(output); }
        | NC ',' NUMBER          { jp_nc_nn(output, $3); }
        | NUMBER                 { jp_nn(output, $1); }
        | NZ ',' NUMBER          { jp_nz_nn(output, $3); }
        | Z ',' NUMBER           { jp_z_nn(output, $3); }
        ;

jr_cmd:
          C ',' NUMBER           { jr_c_n(output, $3); }
        | NC ',' NUMBER          { jr_nc_n(output, $3); }
        | NUMBER                 { jr_n(output, $1); }
        | NZ ',' NUMBER          { jr_nz_n(output, $3); }
        | Z ',' NUMBER           { jr_z_n(output, $3); }
        ;

ld_cmd:
          '(' BC ')' ',' A       { ld_bcp_a(output); }
        | '(' C')' ',' A         { ld_cp_a(output); }
        | '(' DE ')' ',' A       { ld_dep_a(output); }
        | '(' HL ')' ',' A       { ld_hlp_a(output); }
        | '(' HL ')' ',' B       { ld_hlp_b(output); }
        | '(' HL ')' ',' C       { ld_hlp_c(output); }
        | '(' HL ')' ',' D       { ld_hlp_d(output); }
        | '(' HL ')' ',' E       { ld_hlp_e(output); }
        | '(' HL ')' ',' H       { ld_hlp_h(output); }
        | '(' HL ')' ',' L       { ld_hlp_l(output); }
        | '(' HL ')' ',' NUMBER  { ld_hlp_n(output, $5); }
        | '(' NUMBER ')' ',' A   { ld_nnp_a(output, $2); }
        | '(' NUMBER ')' ',' SP  { ld_nnp_sp(output, $2); }
        | A ',' '(' BC ')'       { ld_a_bcp(output); }
        | A ',' '(' C ')'        { ld_a_cp(output); }
        | A ',' '(' DE ')'       { ld_a_dep(output); }
        | A ',' '(' HL ')'       { ld_a_hlp(output); }
        | A ',' '(' NUMBER ')'   { ld_a_nnp(output, $4); }
        | A ',' A                { ld_a_a(output); }
        | A ',' B                { ld_a_b(output); }
        | A ',' C                { ld_a_c(output); }
        | A ',' D                { ld_a_d(output); }
        | A ',' E                { ld_a_e(output); }
        | A ',' H                { ld_a_h(output); }
        | A ',' L                { ld_a_l(output); }
        | A ',' NUMBER           { ld_a_n(output, $3); }
        | B ',' '(' HL ')'       { ld_b_hlp(output); }
        | B ',' A                { ld_b_a(output); }
        | B ',' B                { ld_b_b(output); }
        | B ',' C                { ld_b_c(output); }
        | B ',' D                { ld_b_d(output); }
        | B ',' E                { ld_b_e(output); }
        | B ',' H                { ld_b_h(output); }
        | B ',' L                { ld_b_l(output); }
        | B ',' NUMBER           { ld_b_n(output, $3); }
        | BC ',' NUMBER          { ld_bc_nn(output, $3); }
        | C ',' '(' HL ')'       { ld_c_hlp(output); }
        | C ',' A                { ld_c_a(output); }
        | C ',' B                { ld_c_b(output); }
        | C ',' C                { ld_c_c(output); }
        | C ',' D                { ld_c_d(output); }
        | C ',' E                { ld_c_e(output); }
        | C ',' H                { ld_c_h(output); }
        | C ',' L                { ld_c_l(output); }
        | C ',' NUMBER           { ld_c_n(output, $3); }
        | D ',' '(' HL ')'       { ld_d_hlp(output); }
        | D ',' A                { ld_d_a(output); }
        | D ',' B                { ld_d_b(output); }
        | D ',' C                { ld_d_c(output); }
        | D ',' D                { ld_d_d(output); }
        | D ',' E                { ld_d_e(output); }
        | D ',' H                { ld_d_h(output); }
        | D ',' L                { ld_d_l(output); }
        | D ',' NUMBER           { ld_d_n(output, $3); }
        | DE ',' NUMBER          { ld_de_nn(output, $3); }
        | E ',' '(' HL ')'       { ld_e_hlp(output); }
        | E ',' A                { ld_e_a(output); }
        | E ',' B                { ld_e_b(output); }
        | E ',' C                { ld_e_c(output); }
        | E ',' D                { ld_e_d(output); }
        | E ',' E                { ld_e_e(output); }
        | E ',' H                { ld_e_h(output); }
        | E ',' L                { ld_e_l(output); }
        | E ',' NUMBER           { ld_e_n(output, $3); }
        | H ',' '(' HL ')'       { ld_h_hlp(output); }
        | H ',' A                { ld_h_a(output); }
        | H ',' B                { ld_h_b(output); }
        | H ',' C                { ld_h_c(output); }
        | H ',' D                { ld_h_d(output); }
        | H ',' E                { ld_h_e(output); }
        | H ',' H                { ld_h_h(output); }
        | H ',' L                { ld_h_l(output); }
        | H ',' NUMBER           { ld_h_n(output, $3); }
        | HL ',' NUMBER          { ld_hl_nn(output, $3); }
        | HL ',' SP '+' NUMBER   { ldhl_sp_n(output, $5); }
        | L ',' '(' HL ')'       { ld_l_hlp(output); }
        | L ',' A                { ld_l_a(output); }
        | L ',' B                { ld_l_b(output); }
        | L ',' C                { ld_l_c(output); }
        | L ',' D                { ld_l_d(output); }
        | L ',' E                { ld_l_e(output); }
        | L ',' H                { ld_l_h(output); }
        | L ',' L                { ld_l_l(output); }
        | L ',' NUMBER           { ld_l_n(output, $3); }
        | SP ',' HL              { ld_sp_hl(output); }
        | SP ',' NUMBER          { ld_sp_nn(output, $3); }
        ;

ldd_cmd:
          '(' HL ')' ',' A      { ldd_hlp_a(output); }
        | A ',' '(' HL ')'      { ldd_a_hlp(output); }
        ;

ldh_cmd:
          '(' NUMBER ')' ',' A  { ldh_n_a(output, $2); }
        | A ',' '(' NUMBER ')'  { ldh_a_n(output, $4); }
        ;

ldi_cmd:
          '(' HL ')' ',' A      { ldi_hlp_a(output); }
        | A ',' '(' HL ')'      { ldi_a_hlp(output); }
        ;

or_cmd:
          NUMBER                 { or_n(output, $1); }
        | regs_8                 { orf(output, last_reg); }
        ;

pop_cmd:
          AF                    { pop_af(output); }
        | BC                    { pop_bc(output); }
        | DE                    { pop_de(output); }
        | HL                    { pop_hl(output); }
        ;

push_cmd:
          AF                   { push_af(output); }
        | BC                   { push_bc(output); }
        | DE                   { push_de(output); }
        | HL                   { push_hl(output); }
        ;

ret_cmd:
                                { ret(output); }
        | C                     { ret_c(output); }
        | NC                    { ret_nc(output); }
        | NZ                    { ret_nz(output); }
        | Z                     { ret_z(output); }
        ;

sbc_cmd:
          NUMBER                { sbc_n(output, $1); }
        | regs_8                { sbc(output, last_reg); }
        ;

sub_cmd:
          NUMBER                { sub_n(output, $1); }
        | regs_8                { sub(output, last_reg); }
        ;

xor_cmd:
          NUMBER                { xor_n(output, $1); }
        | regs_8                { xorf(output, last_reg); }
        ;

cb_cmd:
        BIT NUMBER ',' regs_8       { bit(output, $2, last_reg); }
      | RES NUMBER ',' regs_8       { res(output, $2, last_reg); }
      | RL regs_8                   { rl(output, last_reg); }
      | RLC regs_8                  { rlc(output, last_reg); }
      | RR regs_8                   { rr(output, last_reg); }
      | RRC regs_8                  { rrc(output, last_reg); }
      | SET NUMBER ',' regs_8       { set(output, $2, last_reg); }
      | SLA regs_8                  { sla(output, last_reg); }
      | SRA regs_8                  { sra(output, last_reg); }
      | SRL regs_8                  { srl(output, last_reg); }
      | SWAP regs_8                 { swap(output, last_reg); }
      ;

command:
          ADC adc_cmd
        | ADD add_cmd
        | AND and_cmd
        | CALL call_cmd
        | CCF                       { ccf(output); }
        | CP cp_cmd
        | CPL                       { cpl(output); }
        | DAA                       { daa(output); }
        | DATA NUMBER               { data(output, $2); }
        | DEC dec_cmd
        | DI                        { di(output); }
        | EI                        { ei(output); }
        | HALT                      { halt(output); }
        | INC inc_cmd
        | JP jp_cmd
        | JR jr_cmd
        | LD ld_cmd
        | LDD ldd_cmd
        | LDH ldh_cmd
        | LDI ldi_cmd
        | NOP                       { nop(output); }
        | OR or_cmd
        | POP pop_cmd
        | PUSH push_cmd
        | RET ret_cmd
        | RETI                      { reti(output); }
        | RLA                       { rla(output); }
        | RLCA                      { rlca(output); }
        | RRA                       { rra(output); }
        | RRCA                      { rrca(output); }
        | RST NUMBER                { rst(output, $2); }
        | SBC sbc_cmd
        | SCF                       { scf(output); }
        | STOP                      { stop(output); }
        | SUB sub_cmd
        | XOR xor_cmd
        | cb_cmd
        ;
%%

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Syntax: %s <input file> <output file>\n", argv[0]);
        return -1;
    }
    const char *infile = argv[1];
    const char *outfile = argv[2];
    FILE *input = fopen(infile, "r");
    if (!input) {
        fprintf(stderr, "Can't open file %s\n", infile);
        return -1;
    }
    /* Set flex to read from it instead of defaulting to STDIN. */
    yyin = input;
    output = fopen(outfile, "wb");
    if (output == NULL) {
        fprintf(stderr, "Can't open output file %s\n", outfile);
        fclose(input);
        return -1;
    }
    
    /* Parse through the input until there is no more. */
    do {
        yyparse();
    } while (!feof(yyin));

    fclose(input);
    fclose(output);

    return 0;
}

void yyerror(const char *s)
{
    fprintf(stderr, "Parse error:%u: %s: %s\n", linenum, s, yytext);
    exit(EXIT_FAILURE);
}
