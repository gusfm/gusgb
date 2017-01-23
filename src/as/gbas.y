%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* stuff from flex that bison needs to know about */
extern int yylex();
extern FILE *yyin;
 
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
/* Commands */
%token LD
%token LDD
%token ADD

%%

commands:
        | commands command
        ;

command:
          LD A ',' A {printf("01\n");}
        | LD A ',' B {printf("02\n");}
        | LD A ',' C {printf("03\n");}
        ;
%%

int main(void)
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
    
    /* Parse through the input until there is no more. */
    do {
        yyparse();
    } while (!feof(yyin));
    return 0;
}

void yyerror(const char *s)
{
    fprintf(stderr, "parse error: %s\n", s);
    exit(EXIT_FAILURE);
}
