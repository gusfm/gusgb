%{
#include <stdio.h>
#include <string.h>
#include "gbas.tab.h"
unsigned int linenum = 0;
%}
%option noinput nounput

%%
0x[0123456789]+             {yylval.num = strtol(yytext, NULL, 16); return NUMBER;}
$[0123456789]+              {yylval.num = strtol(yytext, NULL, 16); return NUMBER;}
[0123456789]+               {yylval.num = strtol(yytext, NULL, 10); return NUMBER;}
"," |
"(" |
")"                         {return yytext[0];}

adc                         {return ADC;}
add                         {return ADD;}
and                         {return AND;}
call                        {return CALL;}
ccf                         {return CCF;}
cp                          {return CP;}
cpl                         {return CPL;}
daa                         {return DAA;}
data                        {return DATA;}
dec                         {return DEC;}
di                          {return DI;}
ei                          {return EI;}
halt                        {return HALT;}
inc                         {return INC;}
jp                          {return JP;}
jr                          {return JR;}
ld                          {return LD;}
ldd                         {return LDD;}
ldh                         {return LDH;}
ldi                         {return LDI;}
nop                         {return NOP;}
or                          {return OR;}
pop                         {return POP;}
push                        {return PUSH;}
ret                         {return RET;}
reti                        {return RETI;}
rla                         {return RLA;}
rlca                        {return RLCA;}
rra                         {return RRA;}
rrca                        {return RRCA;}
rst                         {return RST;}
sbc                         {return SBC;}
scf                         {return SCF;}
stop                        {return STOP;}
sub                         {return SUB;}
xor                         {return XOR;}

nc                          {return NC;}
nz                          {return NZ;}
z                           {return Z;}

a                           {return A;}
f                           {return F;}
b                           {return B;}
c                           {return C;}
d                           {return D;}
e                           {return E;}
h                           {return H;}
l                           {return L;}
af                          {return AF;}
bc                          {return BC;}
de                          {return DE;}
hl                          {return HL;}
sp                          {return SP;}
\n                          { ++linenum; printf("linenum=%u\n", linenum); }
[ \t]+                      /* ignore white space */;
%%
