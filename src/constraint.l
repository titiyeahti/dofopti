%{
#include "constraint.tab.h"
/*union {char wd[2]; int ival; abr_p aval} yylval;*/
%}
%%
[0-9]+ {yylval.ival = atoi(yytext); return NOMBRE;}
[a-zA-Z]{2} {strcpy(yylval.wd, yytext); return ALIAS;}
%%
