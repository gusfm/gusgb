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
ld                          {return LD;}
ldd                         {return LDD;}
add                         {return ADD;}
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
\n                          { ++linenum; printf("linenum=%u\n", linenum); }
[ \t]+                      /* ignore white space */;
%%
