%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#ifndef NIQUE
#define NIQUE
typedef struct abr{
  int val;
  char symb[2];
  char sign;
  struct abr* f1;
  struct abr* f2;
} abr;
#endif 
%}
%union {char wd[2]; int ival; /*abr * aval;*/}
%token <ival> NOMBRE
%token <wd> ALIAS
%type  expression
%type  terme
%left '&' '|'
%%
calcul : expression {printf("%c %d\n", $1->sign, $1->val);}
       ;

expression: expression '&' expression /*{abr a; a.val = 0; a.sign = '&'; a.symb[0] = '\0'; a.f1 = $1; a.f2 = $3; $$ =  &a;}*/
          | expression '|' expression {abr a; a.val = 0; a.sign = '|'; a.symb[0] = '\0'; a.f1 = $1; a.f2 = $3; $$ =  &a;}
          | '(' expression ')' {abr a; a.val = 0; a.sign = '('; a.symb[0] = '\0'; a.f1 = $2; a.f2=NULL; $$ = &a;}
          | terme
          ;

terme: ALIAS '>' NOMBRE {abr a; a.val = $3; a.sign = '>'; a.f1 = NULL, strcpy(a.symb, $1); a.f1 = NULL; a.f2=NULL; $$= &a;}
     | ALIAS '<' NOMBRE {abr a; a.val = $3; a.sign = '<'; a.f1 = NULL, strcpy(a.symb, $1); a.f1 = NULL; a.f2=NULL; $$= &a;}
     | ALIAS '=' NOMBRE {abr a; a.val = $3; a.sign = '='; a.f1 = NULL, strcpy(a.symb, $1); a.f1 = NULL; a.f2=NULL; $$= &a;}
     ;
%%
int yyerror(void)
{fprintf(stderr, "erreur de syntaxe\n"); return 1;}
