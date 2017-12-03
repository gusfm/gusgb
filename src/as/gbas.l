D           [0-9]
L           [a-zA-Z_]
A           [a-zA-Z_0-9]
H           [a-fA-F0-9]
%{
#include <stdio.h>
#include <string.h>
#include "gbas.tab.h"
%}
%option noinput nounput

%%
${H}+                       { yylval.num = strtol(yytext + 1, NULL, 16); return NUMBER; }
{D}+                        { yylval.num = strtol(yytext, NULL, 10); return NUMBER; }
L?\"(\\.|[^\\"])*\"         { yylval.sval = strdup(yytext); return STRING_LITERAL; }
";".*                       { /* ignore comments */ };
"\n"                        {}

"+" |
"-" |
"." |
"," |
":" |
"(" |
")"                         { return yytext[0]; }

ascii                       { return ASCII; }
db                          { return DB; }
fill                        { return FILL; }
org                         { return ORG; }

adc                         { return ADC; }
add                         { return ADD; }
and                         { return AND; }
call                        { return CALL; }
ccf                         { return CCF; }
cp                          { return CP; }
cpl                         { return CPL; }
daa                         { return DAA; }
dec                         { return DEC; }
di                          { return DI; }
ei                          { return EI; }
halt                        { return HALT; }
inc                         { return INC; }
jp                          { return JP; }
jr                          { return JR; }
ld                          { return LD; }
nop                         { return NOP; }
or                          { return OR; }
pop                         { return POP; }
push                        { return PUSH; }
ret                         { return RET; }
reti                        { return RETI; }
rla                         { return RLA; }
rlca                        { return RLCA; }
rra                         { return RRA; }
rrca                        { return RRCA; }
rst                         { return RST; }
sbc                         { return SBC; }
scf                         { return SCF; }
stop                        { return STOP; }
sub                         { return SUB; }
xor                         { return XOR; }

bit                         { return BIT; }
res                         { return RES; }
rl                          { return RL; }
rlc                         { return RLC; }
rr                          { return RR; }
rrc                         { return RRC; }
set                         { return SET; }
sla                         { return SLA; }
sra                         { return SRA; }
srl                         { return SRL; }
swap                        { return SWAP; }

nc                          { return NC; }
nz                          { return NZ; }
z                           { return Z; }

a                           { return A; }
f                           { return F; }
b                           { return B; }
c                           { return C; }
d                           { return D; }
e                           { return E; }
h                           { return H; }
l                           { return L; }
af                          { return AF; }
bc                          { return BC; }
de                          { return DE; }
hl                          { return HL; }
sp                          { return SP; }
{L}{A}*                     { yylval.sval = strdup(yytext); return LABEL; }
[ \t]+                      /* ignore white space */;
%%
