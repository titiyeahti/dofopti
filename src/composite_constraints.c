#include "composite_constraints.h"

int str_to_statid(char* s){
  int i;
  for(i=0; i<STATS_COUNT; i++)
    if(!strcmp(s, stats_names[i])) return i;

  ERR_MSG("unrecognized symbol");
  exit(1);
}

int trim_spaces(char* input, char buffer[]){
  int i;

  if(strlen(input) + 1 > BUFF_LEN) {
    ERR_MSG("expression is too long.");
    exit(1);
  }

  int j = 0;
  for(i=0; i<strlen(input); i++){
    if(input[i] == ' ' || input[i] == '\t')
      continue;

    buffer[j] = input[i];
    j++;
  }

  buffer[j] = '\0';

  return 0;
}

compconst_s* new_double(double d){
  compconst_s* res = (compconst_s*) malloc(sizeof(compconst_s));

  res->t = DOUBLE;
  res->value = d;

  return res;
}

compconst_s* new_lit(int i){
  compconst_s* res = (compconst_s*) malloc(sizeof(compconst_s));

  res->t = LIT;
  res->lit = i;

  return res;
}

compconst_s* new_braces(compconst_s** c){
  compconst_s* res = (compconst_s*) malloc(sizeof(compconst_s));

  res->t = BRACES;
  res->braces = &c;

  return res;
}

compconst_s* new_node(int type, compconst_s** lm, compconst_s** rm){
  if(t != ADD && t != MULT && t != MINUS){
    ERR_MSG("illegal node type.");
    exit(1);
  }

  res->t = t;
  res->node.lm = *lm;
  res->node.rm = *rm;

  return res;
}

/*recursively read the string and create a probably incorrect tree 
 * with a long right branch*/
/* TODO implement the unary minus : if starts with '-', 
 * return (0 - rec(buf+1, len-1))*/
const_expr_s* aux_ccfromstr(char buffer[], int len){
  compconst_s * res;

  compconst_s * tmp;
  compconst_s * lm;
  compconst_s * rm;

  int count;
  int t;

  if(buffer[0] == '('){
    int braces_count = 1;
    count = 1;
    while (braces_count && count < len) {
      count ++;
      if(buffer[count] == ')') braces_count --;
      if(buffer[count] == '(') braces_count ++;
    }


    tmp = aux_ccfromstr(buffer+1, count-1);
    lm = new_braces(&tmp);

  }

  if(buffer[0] <= 'z' && buffer[0] >= 'a'){
    char symb[4];
    count = 0;
    while(buffer[count] <= 'z' && buffer[count] >= 'a' && count < 4){
      symb[count] = buffer[count];
      count ++;
    }

    symb[count] = '\0';

    int statid = str_to_statid(symb);

    lm = new_lit(statid);
  }

  if(buffer[0] <= '9' && buffer[0] >= '9'){
    /*strtod*/
    /*WARNING : might be some not null terminated string issues down there*/
    char* endptr;
    double val;
    val = strtod(buffer, &endptr);
    count = (int)(endptr - buffer) - 1;

    lm = new_double(val);
  }
  
  if (count == len-1) 
    return lm;

  switch buffer[count + 1] {
    case '+' :
      t = ADD;
      break;

    case '*' :
      t = MULT;
      break;

    case '-' :
      t = MINUS;
      break;

    default :
      ERR_MSG("wrong operator");
  }

  rs = count + 2;

  rm = aux_ccfromstr(buffer+count+2, len-c-2);
  res = new_node(t, &lm, &rm);
  return res;
}

const_epxr_s* compconst_from_str(const char* str){
  if(!strlen(str)) return NULL;
  compconst_s * res;
  compconst_s * tmp;

  char buffer[BUFF_LEN];
  trim_spaces(str, buffer);
  tmp = aux_ccfromstr(buffer, strlen(buffer));
}

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
int compconst_eval(compconst_s* ce, double coeffs[], double* const_term);

void free_compconst(compconst_s** ce){
  compconst_s* temp = *ce;
  if (!temp) return;

  switch (temp->t) {
    case DOUBLE :
      break;
    case LIT :
      break;

    case BRACES :
      if(temp->braces) free_compconst(&temp->braces);
      break;

    case ADD:
    case MULT:
      if(temp->node.lm) free_compconst(&temp->node.lm);
      if(temp->node.rm) free_compconst(&temp->node.rm);

    default :
      break;
  }

  free(temp);
  *ct = NULL;
}
