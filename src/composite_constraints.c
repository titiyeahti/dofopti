#include "composite_constraints.h"

const_expr_s* new_double(double d){
  const_expr_s* res = (const_expr_s*) malloc(sizeof(const_expr_s));

  res->t = DOUBLE;
  res->value = d;

  return res;
}

const_expr_s* new_lit(int i){
  const_expr_s* res = (const_expr_s*) malloc(sizeof(const_expr_s));

  res->t = LIT;
  res->lit = i;

  return res;
}

const_expr_s* new_braces(const_expr_s** c){
  const_expr_s* res = (const_expr_s*) malloc(sizeof(const_expr_s));

  res->t = BRACES;
  res->braces = &c;

  return res;


const_expr_s* new_node(int type, const_expr_s** lm, const_expr_s** rm){
  if(t != ADD && t != MULT){
    ERR_MSG("illegal node type");
    exit(1);
  }

  res->t = t;
  res->node.lm = *lm;
  res->node.rm = *rm;

  return res;
}

const_epxr_s* cons_expr_from_str(const char* str);

/* 0.5*0.01*((12 + 14)*0.3 + (15+18)*(1-0.3))*(stat  + pui  + 100) + do + do elem + (1-0.3)*docri
 * lit -> expr
double -> double
double +/- double -> double
double * double -> double
(double) -> double
double * expr -> expr
(expr) -> expr
expr + expr -> expr 
expr - expr -> epxr */
int const_expr_eval(const_expr_s* ce, double coeffs[], double* const_term);


