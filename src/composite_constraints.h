#ifndef comp_const_H
#define comp_const_H

#include "data_types.h"
#include "glpapi.c"

int str_to_statid(char* s);
int trim_spaces(char* input, char buffer[]);
compconst_s* cc_new_double(double d);
compconst_s* cc_new_lit(int i);
compconst_s* cc_new_braces(compconst_s** c);
compconst_s* cc_new_node(int type, compconst_s** lm, compconst_s** rm);
:q

const_epxr_s* compconst_from_str(const char* str);

int compconst_eval(compconst_s* ce, double coeffs[], double* const_term);

void free_compconst(compconst_s** ce);


#endif
