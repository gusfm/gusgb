%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "as/opcodes.h"

/* stuff from flex that bison needs to know about */
extern int yylex();
extern FILE *yyin;
FILE *output;
 
void yyerror(const char *s);
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

/* Sub commands. */
%token NC
%token NZ
%token Z

%%

commands:
        | commands command
        ;

adc_cmd:
          NUMBER                { adc_n(output, $1); }
        | '(' HL ')'            { adc_hlp(output); }
        | A                     { adc_a(output); }
        | B                     { adc_b(output); }
        | C                     { adc_c(output); }
        | D                     { adc_d(output); }
        | E                     { adc_e(output); }
        | H                     { adc_h(output); }
        | L                     { adc_l(output); }
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
          '(' HL ')'            { and_hlp(output); }
        | A                     { and_a(output); }
        | B                     { and_b(output); }
        | C                     { and_c(output); }
        | D                     { and_d(output); }
        | E                     { and_e(output); }
        | H                     { and_h(output); }
        | L                     { and_l(output); }
        | NUMBER                { and_n(output, $1); }
        ;

call_cmd:
          C ',' NUMBER         { call_c_nn(output, $3); }
        | NC ',' NUMBER        { call_nc_nn(output, $3); }
        | NUMBER               { call_nn(output, $1); }
        | NZ ',' NUMBER        { call_nz_nn(output, $3); }
        | Z ',' NUMBER         { call_z_nn(output, $3); }
        ;

cp_cmd:
          '(' HL ')'             { cp_hlp(output); }
        | A                      { cp_a(output); }
        | B                      { cp_b(output); }
        | C                      { cp_c(output); }
        | D                      { cp_d(output); }
        | E                      { cp_e(output); }
        | H                      { cp_h(output); }
        | L                      { cp_l(output); }
        | NUMBER                 { cp_n(output, $1); }
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
          '(' HL ')'             { or_hlp(output); }
        | A                      { or_a(output); }
        | B                      { or_b(output); }
        | C                      { or_c(output); }
        | D                      { or_d(output); }
        | E                      { or_e(output); }
        | H                      { or_h(output); }
        | L                      { or_l(output); }
        | NUMBER                 { or_n(output, $1); }
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
          '(' HL ')'            { sbc_hlp(output); }
        | A                     { sbc_a(output); }
        | B                     { sbc_b(output); }
        | C                     { sbc_c(output); }
        | D                     { sbc_d(output); }
        | E                     { sbc_e(output); }
        | H                     { sbc_h(output); }
        | L                     { sbc_l(output); }
        | NUMBER                { sbc_n(output, $1); }
        ;

sub_cmd:
          '(' HL ')'            { sub_hlp(output); }
        | A                     { sub_a(output); }
        | B                     { sub_b(output); }
        | C                     { sub_c(output); }
        | D                     { sub_d(output); }
        | E                     { sub_e(output); }
        | H                     { sub_h(output); }
        | L                     { sub_l(output); }
        | NUMBER                { sub_n(output, $1); }
        ;

xor_cmd:
          '(' HL ')'            { xor_hlp(output); }
        | A                     { xor_a(output); }
        | B                     { xor_b(output); }
        | C                     { xor_c(output); }
        | D                     { xor_d(output); }
        | E                     { xor_e(output); }
        | H                     { xor_h(output); }
        | L                     { xor_l(output); }
        | NUMBER                { xor_n(output, $1); }
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
        ;
%%

int main(int argc, char **argv)
{
#if 0
    // open a file handle to a particular file:
    FILE *myfile = fopen("a.snazzle.file", "r");
    // make sure it is valid:
    if (!myfile) {
        cout << "I can't open a.snazzle.file!" << endl;
        return -1;
    }
    // set flex to read from it instead of defaulting to STDIN:
    yyin = myfile;
#endif
    output = fopen("out.gb", "wb");
    if (output == NULL) {
        fprintf(stderr, "cant open output file!\n");
        return -1;
    }
    
    /* Parse through the input until there is no more. */
    do {
        yyparse();
    } while (!feof(yyin));

    fclose(output);

    return 0;
}

void yyerror(const char *s)
{
    fprintf(stderr, "parse error: %s\n", s);
    exit(EXIT_FAILURE);
}
