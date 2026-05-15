#ifndef comp_const_H
#define comp_const_H

#include "data_types.h"
#include "glpapi.c"
#include "constree.h"

const_expr_s* new_double(double d);
const_expr_s* new_lit(int i);
const_expr_s* new_braces(const_expr_s** c);
const_expr_s* new_node(int type, const_expr_s** lm, const_expr_s** rm);

const_epxr_s* cons_expr_from_str(const char* str);

int const_expr_eval(const_expr_s* ce, double coeffs[], double* const_term);



#endif
