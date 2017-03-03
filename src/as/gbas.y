%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "as/gbas.h"
#include "as/opcodes.h"
#include "cart.h"

/* stuff from flex that bison needs to know about */
extern int yylex();
extern FILE *yyin;
FILE *output;
extern unsigned int linenum;
extern const char *yytext;
void yyerror(const char *s);

gbas_t *gbas;
register_e last_reg;
int jump_addr;
char *jump_label;
unsigned int pc = 0;

#define JP() { if (jump_label) gbas_jump_insert(gbas, jump_label, pc, JUMP_JP); }
#define JR() { if (jump_label) gbas_jump_insert(gbas, jump_label, pc, JUMP_JR); }

%}

/* */
%union {
    unsigned int num;
    char *sval;
}

%token <num> NUMBER
%token <sval> STRING_LITERAL
%token <sval> LABEL
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
%token SEEK
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
      ;

regs_8_hlp:
       regs_8
      | '(' HL ')'    { last_reg = REG_HL; }
      ;

adc_cmd:
          NUMBER                { adc_n($1); }
        | regs_8_hlp            { adc(last_reg); }
        ;

add_a:
       NUMBER                   { add_a_n($1); }
     | regs_8_hlp               { add_a_reg(last_reg); }
     ;

add_cmd:
         A ',' add_a
       | HL ',' BC             { add_hl_bc(); }
       | HL ',' DE             { add_hl_de(); }
       | HL ',' HL             { add_hl_hl(); }
       | HL ',' SP             { add_hl_sp(); }
       | SP ',' NUMBER         { add_sp_n($3); }
       ;

and_cmd:
          NUMBER                { and_n($1); }
        | regs_8_hlp            { andf(last_reg); }
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
        | regs_8_hlp             { cp(last_reg); }
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

jump_to:
         NUMBER                 { jump_label = NULL; jump_addr = $1; }
       | LABEL                  { jump_label = $1; jump_addr = 0x00; }
       ;

jp_cmd:
          C ',' jump_to         { JP(); jp_c_nn(jump_addr); }
        | HL                    { JP(); jp_hl(); }
        | NC ',' jump_to        { JP(); jp_nc_nn(jump_addr); }
        | jump_to               { JP(); jp_nn(jump_addr); }
        | NZ ',' jump_to        { JP(); jp_nz_nn(jump_addr); }
        | Z ',' jump_to         { JP(); jp_z_nn(jump_addr); }
        ;

jr_cmd:
          C ',' jump_to         { JR(); jr_c_n((int8_t)(jump_addr)); }
        | NC ',' jump_to        { JR(); jr_nc_n((int8_t)(jump_addr)); }
        | jump_to               { JR(); jr_n((int8_t)(jump_addr)); }
        | NZ ',' jump_to        { JR(); jr_nz_n((int8_t)(jump_addr)); }
        | Z ',' jump_to         { JR(); jr_z_n((int8_t)(jump_addr)); }
        ;

ld_a:
      NUMBER                     { ld_a_n($1); }
    | regs_8_hlp                 { ld_a_reg(last_reg); }
    ;

ld_b:
      NUMBER                     { ld_b_n($1); }
    | regs_8_hlp                 { ld_b_reg(last_reg); }
    ;

ld_c:
      NUMBER                     { ld_c_n($1); }
    | regs_8_hlp                 { ld_c_reg(last_reg); }
    ;

ld_d:
      NUMBER                     { ld_d_n($1); }
    | regs_8_hlp                 { ld_d_reg(last_reg); }
    ;

ld_e:
      NUMBER                     { ld_e_n($1); }
    | regs_8_hlp                 { ld_e_reg(last_reg); }
    ;

ld_h:
      NUMBER                     { ld_h_n($1); }
    | regs_8_hlp                 { ld_h_reg(last_reg); }
    ;

ld_l:
      NUMBER                     { ld_l_n($1); }
    | regs_8_hlp                 { ld_l_reg(last_reg); }
    ;

ld_hlp:
      NUMBER                     { ld_hlp_n($1); }
    | regs_8                     { ld_hlp_reg(last_reg); }
    ;

ld_cmd:
          '(' BC ')' ',' A       { ld_bcp_a(); }
        | '(' DE ')' ',' A       { ld_dep_a(); }
        | '(' HL ')' ',' ld_hlp
        | '(' HL '+' ')' ',' A   { ldi_hlp_a(); }
        | '(' HL '-' ')' ',' A   { ldd_hlp_a(); }
        | '(' NUMBER ')' ',' A   { ld_nnp_a($2); }
        | '(' NUMBER ')' ',' SP  { ld_nnp_sp($2); }
        | '(' NUMBER '+' C')' ',' A { ld_cp_a($2); }
        | '(' NUMBER '+' NUMBER ')' ',' A { ldh_n_a($2, $4); }
        | A ',' '(' BC ')'       { ld_a_bcp(); }
        | A ',' '(' DE ')'       { ld_a_dep(); }
        | A ',' '(' HL '+' ')'   { ldi_a_hlp(); }
        | A ',' '(' HL '-' ')'   { ldd_a_hlp(); }
        | A ',' '(' NUMBER ')'   { ld_a_nnp($4); }
        | A ',' '(' NUMBER '+' C ')' { ld_a_cp($4); }
        | A ',' '(' NUMBER '+' NUMBER ')' { ldh_a_n($4, $6); }
        | A ',' ld_a
        | B ',' ld_b
        | C ',' ld_c
        | D ',' ld_d
        | E ',' ld_e
        | H ',' ld_h
        | L ',' ld_l
        | BC ',' NUMBER          { ld_bc_nn($3); }
        | DE ',' NUMBER          { ld_de_nn($3); }
        | HL ',' NUMBER          { ld_hl_nn($3); }
        | HL ',' SP '+' NUMBER   { ldhl_sp_n($5); }
        | SP ',' HL              { ld_sp_hl(); }
        | SP ',' NUMBER          { ld_sp_nn($3); }
        ;

or_cmd:
          NUMBER                { or_n($1); }
        | regs_8_hlp            { orf(last_reg); }
        ;

pop_cmd:
          AF                    { pop_af(); }
        | BC                    { pop_bc(); }
        | DE                    { pop_de(); }
        | HL                    { pop_hl(); }
        ;

push_cmd:
          AF                    { push_af(); }
        | BC                    { push_bc(); }
        | DE                    { push_de(); }
        | HL                    { push_hl(); }
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
        | regs_8_hlp            { sbc(last_reg); }
        ;

sub_cmd:
          NUMBER                { sub_n($1); }
        | regs_8_hlp            { sub(last_reg); }
        ;

xor_cmd:
          NUMBER                { xor_n($1); }
        | regs_8_hlp            { xorf(last_reg); }
        ;

cb_cmd:
        BIT NUMBER ',' regs_8_hlp       { bit($2, last_reg); }
      | RES NUMBER ',' regs_8_hlp       { res($2, last_reg); }
      | RL regs_8_hlp                   { rl(last_reg); }
      | RLC regs_8_hlp                  { rlc(last_reg); }
      | RR regs_8_hlp                   { rr(last_reg); }
      | RRC regs_8_hlp                  { rrc(last_reg); }
      | SET NUMBER ',' regs_8_hlp       { set($2, last_reg); }
      | SLA regs_8_hlp                  { sla(last_reg); }
      | SRA regs_8_hlp                  { sra(last_reg); }
      | SRL regs_8_hlp                  { srl(last_reg); }
      | SWAP regs_8_hlp                 { swap(last_reg); }
      ;

command:
          '.' ASCII STRING_LITERAL  { ascii($3); free($3); }
        | '.' DATA NUMBER           { data($3); }
        | '.' SEEK NUMBER           { seek($3); }
        | '.' MEMSET NUMBER ',' NUMBER { memsetf($3, $5); }
        | LABEL ':'                 { gbas_label_insert(gbas, $1, pc); }
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
    gbas = gbas_init(argc, argv);
    if (gbas == NULL) {
        return -1;
    }
    /* Set flex to read from it instead of defaulting to STDIN. */
    yyin = gbas->input;
    output = gbas->output;
    /* Parse through the input until there is no more. */
    do {
        yyparse();
    } while (!feof(yyin));
    /* Write all jump values. */
    gbas_jump_process(gbas);
    /* Output cartridge header. */
    if (gbas->gen_header) {
        cart_header_t header;
        cart_header_init(&header, gbas->outfile);
        cart_header_write(&header, output);
    }
    gbas_finish(gbas);
    return 0;
}

void yyerror(const char *s)
{
    fprintf(stderr, "Parse error:%u: %s: %s\n", linenum, s, yytext);
    exit(EXIT_FAILURE);
}
