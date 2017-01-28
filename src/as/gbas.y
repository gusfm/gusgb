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
%token <sval> STRING_LITERAL
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

/* Special compiller commands. */
%token ASCII
%token DATA
%token JUMP
%token MEMSET

/* Commands */
%token ADC
%token ADD
%token AND
%token CALL
%token CCF
%token CP
%token CPL
%token DAA
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
          NUMBER                { adc_n($1); }
        | regs_8                { adc(last_reg); }
        ;

add_cmd:
          A                     { add_a_a(); }
        | A ',' '(' HL ')'      { add_a_hlp(); }
        | A ',' B               { add_a_b(); }
        | A ',' C               { add_a_c(); }
        | A ',' D               { add_a_d(); }
        | A ',' E               { add_a_e(); }
        | A ',' H               { add_a_h(); }
        | A ',' L               { add_a_l(); }
        | A ',' NUMBER          { add_a_n($3); }
        | HL ',' BC             { add_hl_bc(); }
        | HL ',' DE             { add_hl_de(); }
        | HL ',' HL             { add_hl_hl(); }
        | HL ',' SP             { add_hl_sp(); }
        | SP ',' NUMBER         { add_sp_n($3); }
        ;

and_cmd:
          NUMBER                { and_n($1); }
        | regs_8                { andf(last_reg); }
        ;

call_cmd:
          C ',' NUMBER         { call_c_nn($3); }
        | NC ',' NUMBER        { call_nc_nn($3); }
        | NUMBER               { call_nn($1); }
        | NZ ',' NUMBER        { call_nz_nn($3); }
        | Z ',' NUMBER         { call_z_nn($3); }
        ;

cp_cmd:
          NUMBER                 { cp_n($1); }
        | regs_8                 { cp(last_reg); }
        ;

dec_cmd:
          '(' HL ')'            { dec_hlp(); }
        | A                     { dec_a(); }
        | B                     { dec_b(); }
        | BC                    { dec_bc(); }
        | C                     { dec_c(); }
        | D                     { dec_d(); }
        | DE                    { dec_de(); }
        | E                     { dec_e(); }
        | H                     { dec_h(); }
        | HL                    { dec_hl(); }
        | L                     { dec_l(); }
        | SP                    { dec_sp(); }
        ;

inc_cmd:
          '(' HL ')'            { inc_hlp(); }
        | A                     { inc_a(); }
        | B                     { inc_b(); }
        | BC                    { inc_bc(); }
        | C                     { inc_c(); }
        | D                     { inc_d(); }
        | DE                    { inc_de(); }
        | E                     { inc_e(); }
        | H                     { inc_h(); }
        | HL                    { inc_hl(); }
        | L                     { inc_l(); }
        | SP                    { inc_sp(); }
        ;

jp_cmd:
          C ',' NUMBER           { jp_c_nn($3); }
        | HL                     { jp_hl(); }
        | NC ',' NUMBER          { jp_nc_nn($3); }
        | NUMBER                 { jp_nn($1); }
        | NZ ',' NUMBER          { jp_nz_nn($3); }
        | Z ',' NUMBER           { jp_z_nn($3); }
        ;

jr_cmd:
          C ',' NUMBER           { jr_c_n($3); }
        | NC ',' NUMBER          { jr_nc_n($3); }
        | NUMBER                 { jr_n($1); }
        | NZ ',' NUMBER          { jr_nz_n($3); }
        | Z ',' NUMBER           { jr_z_n($3); }
        ;

ld_cmd:
          '(' BC ')' ',' A       { ld_bcp_a(); }
        | '(' C')' ',' A         { ld_cp_a(); }
        | '(' DE ')' ',' A       { ld_dep_a(); }
        | '(' HL ')' ',' A       { ld_hlp_a(); }
        | '(' HL ')' ',' B       { ld_hlp_b(); }
        | '(' HL ')' ',' C       { ld_hlp_c(); }
        | '(' HL ')' ',' D       { ld_hlp_d(); }
        | '(' HL ')' ',' E       { ld_hlp_e(); }
        | '(' HL ')' ',' H       { ld_hlp_h(); }
        | '(' HL ')' ',' L       { ld_hlp_l(); }
        | '(' HL ')' ',' NUMBER  { ld_hlp_n($5); }
        | '(' NUMBER ')' ',' A   { ld_nnp_a($2); }
        | '(' NUMBER ')' ',' SP  { ld_nnp_sp($2); }
        | A ',' '(' BC ')'       { ld_a_bcp(); }
        | A ',' '(' C ')'        { ld_a_cp(); }
        | A ',' '(' DE ')'       { ld_a_dep(); }
        | A ',' '(' HL ')'       { ld_a_hlp(); }
        | A ',' '(' NUMBER ')'   { ld_a_nnp($4); }
        | A ',' A                { ld_a_a(); }
        | A ',' B                { ld_a_b(); }
        | A ',' C                { ld_a_c(); }
        | A ',' D                { ld_a_d(); }
        | A ',' E                { ld_a_e(); }
        | A ',' H                { ld_a_h(); }
        | A ',' L                { ld_a_l(); }
        | A ',' NUMBER           { ld_a_n($3); }
        | B ',' '(' HL ')'       { ld_b_hlp(); }
        | B ',' A                { ld_b_a(); }
        | B ',' B                { ld_b_b(); }
        | B ',' C                { ld_b_c(); }
        | B ',' D                { ld_b_d(); }
        | B ',' E                { ld_b_e(); }
        | B ',' H                { ld_b_h(); }
        | B ',' L                { ld_b_l(); }
        | B ',' NUMBER           { ld_b_n($3); }
        | BC ',' NUMBER          { ld_bc_nn($3); }
        | C ',' '(' HL ')'       { ld_c_hlp(); }
        | C ',' A                { ld_c_a(); }
        | C ',' B                { ld_c_b(); }
        | C ',' C                { ld_c_c(); }
        | C ',' D                { ld_c_d(); }
        | C ',' E                { ld_c_e(); }
        | C ',' H                { ld_c_h(); }
        | C ',' L                { ld_c_l(); }
        | C ',' NUMBER           { ld_c_n($3); }
        | D ',' '(' HL ')'       { ld_d_hlp(); }
        | D ',' A                { ld_d_a(); }
        | D ',' B                { ld_d_b(); }
        | D ',' C                { ld_d_c(); }
        | D ',' D                { ld_d_d(); }
        | D ',' E                { ld_d_e(); }
        | D ',' H                { ld_d_h(); }
        | D ',' L                { ld_d_l(); }
        | D ',' NUMBER           { ld_d_n($3); }
        | DE ',' NUMBER          { ld_de_nn($3); }
        | E ',' '(' HL ')'       { ld_e_hlp(); }
        | E ',' A                { ld_e_a(); }
        | E ',' B                { ld_e_b(); }
        | E ',' C                { ld_e_c(); }
        | E ',' D                { ld_e_d(); }
        | E ',' E                { ld_e_e(); }
        | E ',' H                { ld_e_h(); }
        | E ',' L                { ld_e_l(); }
        | E ',' NUMBER           { ld_e_n($3); }
        | H ',' '(' HL ')'       { ld_h_hlp(); }
        | H ',' A                { ld_h_a(); }
        | H ',' B                { ld_h_b(); }
        | H ',' C                { ld_h_c(); }
        | H ',' D                { ld_h_d(); }
        | H ',' E                { ld_h_e(); }
        | H ',' H                { ld_h_h(); }
        | H ',' L                { ld_h_l(); }
        | H ',' NUMBER           { ld_h_n($3); }
        | HL ',' NUMBER          { ld_hl_nn($3); }
        | HL ',' SP '+' NUMBER   { ldhl_sp_n($5); }
        | L ',' '(' HL ')'       { ld_l_hlp(); }
        | L ',' A                { ld_l_a(); }
        | L ',' B                { ld_l_b(); }
        | L ',' C                { ld_l_c(); }
        | L ',' D                { ld_l_d(); }
        | L ',' E                { ld_l_e(); }
        | L ',' H                { ld_l_h(); }
        | L ',' L                { ld_l_l(); }
        | L ',' NUMBER           { ld_l_n($3); }
        | SP ',' HL              { ld_sp_hl(); }
        | SP ',' NUMBER          { ld_sp_nn($3); }
        ;

ldd_cmd:
          '(' HL ')' ',' A      { ldd_hlp_a(); }
        | A ',' '(' HL ')'      { ldd_a_hlp(); }
        ;

ldh_cmd:
          '(' NUMBER ')' ',' A  { ldh_n_a($2); }
        | A ',' '(' NUMBER ')'  { ldh_a_n($4); }
        ;

ldi_cmd:
          '(' HL ')' ',' A      { ldi_hlp_a(); }
        | A ',' '(' HL ')'      { ldi_a_hlp(); }
        ;

or_cmd:
          NUMBER                 { or_n($1); }
        | regs_8                 { orf(last_reg); }
        ;

pop_cmd:
          AF                    { pop_af(); }
        | BC                    { pop_bc(); }
        | DE                    { pop_de(); }
        | HL                    { pop_hl(); }
        ;

push_cmd:
          AF                   { push_af(); }
        | BC                   { push_bc(); }
        | DE                   { push_de(); }
        | HL                   { push_hl(); }
        ;

ret_cmd:
                                { ret(); }
        | C                     { ret_c(); }
        | NC                    { ret_nc(); }
        | NZ                    { ret_nz(); }
        | Z                     { ret_z(); }
        ;

sbc_cmd:
          NUMBER                { sbc_n($1); }
        | regs_8                { sbc(last_reg); }
        ;

sub_cmd:
          NUMBER                { sub_n($1); }
        | regs_8                { sub(last_reg); }
        ;

xor_cmd:
          NUMBER                { xor_n($1); }
        | regs_8                { xorf(last_reg); }
        ;

cb_cmd:
        BIT NUMBER ',' regs_8       { bit($2, last_reg); }
      | RES NUMBER ',' regs_8       { res($2, last_reg); }
      | RL regs_8                   { rl(last_reg); }
      | RLC regs_8                  { rlc(last_reg); }
      | RR regs_8                   { rr(last_reg); }
      | RRC regs_8                  { rrc(last_reg); }
      | SET NUMBER ',' regs_8       { set($2, last_reg); }
      | SLA regs_8                  { sla(last_reg); }
      | SRA regs_8                  { sra(last_reg); }
      | SRL regs_8                  { srl(last_reg); }
      | SWAP regs_8                 { swap(last_reg); }
      ;

command:
          '.' ASCII STRING_LITERAL  { ascii($3); free($3); }
        | '.' DATA NUMBER           { data($3); }
        | '.' JUMP NUMBER           { jump($3); }
        | '.' MEMSET NUMBER ',' NUMBER { memsetf($3, $5); }
        | ADC adc_cmd
        | ADD add_cmd
        | AND and_cmd
        | CALL call_cmd
        | CCF                       { ccf(); }
        | CP cp_cmd
        | CPL                       { cpl(); }
        | DAA                       { daa(); }
        | DEC dec_cmd
        | DI                        { di(); }
        | EI                        { ei(); }
        | HALT                      { halt(); }
        | INC inc_cmd
        | JP jp_cmd
        | JR jr_cmd
        | LD ld_cmd
        | LDD ldd_cmd
        | LDH ldh_cmd
        | LDI ldi_cmd
        | NOP                       { nop(); }
        | OR or_cmd
        | POP pop_cmd
        | PUSH push_cmd
        | RET ret_cmd
        | RETI                      { reti(); }
        | RLA                       { rla(); }
        | RLCA                      { rlca(); }
        | RRA                       { rra(); }
        | RRCA                      { rrca(); }
        | RST NUMBER                { rst($2); }
        | SBC sbc_cmd
        | SCF                       { scf(); }
        | STOP                      { stop(); }
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
