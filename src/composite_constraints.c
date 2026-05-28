#include "composite_constraints.h"

#define DEF(e, str, s) s
const char * const stats_symbols[] = {
#include "stats.conf"
};
#undef DEF

int str_to_statid(char* s){
  int i;
  for(i=0; i<STATS_COUNT; i++)
    if(!strcmp(s, stats_symbols[i])) return i;

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

compconst_s* cc_new_double(double d){
  compconst_s* res = (compconst_s*) malloc(sizeof(compconst_s));

  res->t = DOUBLE;
  res->value = d;

  return res;
}

compconst_s* cc_new_lit(int i){
  compconst_s* res = (compconst_s*) malloc(sizeof(compconst_s));

  res->t = LIT;
  res->lit = i;

  return res;
}

compconst_s* cc_new_braces(compconst_s** c){
  compconst_s* res = (compconst_s*) malloc(sizeof(compconst_s));

  res->t = BRACES;
  res->braces = *c;

  return res;
}

compconst_s* cc_new_node(int t, compconst_s** lm, compconst_s** rm){
  if(t != ADD && t != MULT && t != SUB){
    ERR_MSG("illegal node type.");
    exit(1);
  }
  compconst_s* res = (compconst_s*) malloc(sizeof(compconst_s));

  res->t = t;
  res->node.lm = *lm;
  res->node.rm = *rm;

  return res;
}

/*recursively read the string and create a probably incorrect tree 
 * with a long right branch*/
/* TODO implement the unary minus : if starts with '-', 
 * return (0 - rec(buf+1, len-1))*/
compconst_s* aux_ccfromstr(char buffer[], int len){
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
    lm = cc_new_braces(&tmp);

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

    lm = cc_new_lit(statid);
    count--;
  }

  if((buffer[0] <= '9' && buffer[0] >= '0')||buffer[0] == '.'){
    /*strtod*/
    /*WARNING : might be some not null terminated string issues down there*/
    char* endptr;
    double val;
    val = strtod(buffer, &endptr);
    count = (int)(endptr - buffer) - 1;

    lm = cc_new_double(val);
  }

  if (count == len-1) 
    return lm;

  switch (buffer[count + 1]) {
    case '+' :
      t = ADD;
      break;

    case '*' :
      t = MULT;

      break;

    case '-' :
      t = SUB;
      break;

    default :
      ERR_MSG("wrong operator");
      exit(1);
  }

  if (t==SUB) {
    compconst_s* c1 = cc_new_double(-1.);
    compconst_s* c2 = aux_ccfromstr(buffer+count+2, len-count-2);
    rm = cc_new_node(MULT, &c1, &c2);
    t = ADD;
  }
  else{
    rm = aux_ccfromstr(buffer+count+2, len-count-2);
  }
  res = cc_new_node(t, &lm, &rm);
  return res;
}

compconst_s* aux_fix_cc(compconst_s* cc){

  switch (cc->t) {
    case DOUBLE :
      return cc;
    case LIT :
      return cc;
    case BRACES :
      cc->braces = aux_fix_cc(cc->braces);
      return cc;
    case SUB :
    case ADD :
      cc->node.lm = aux_fix_cc(cc->node.lm);
      cc->node.rm = aux_fix_cc(cc->node.rm);
      return cc;

    case MULT :
      /* look for an eventual +/- in rm */
      /* rebranch acchordingly */
      /* recursivre call in left and right childs */
      compconst_s* cursor = cc;
      compconst_s* last;
      while (cursor->t == MULT){ 
        last = cursor;
        cursor = cursor->node.rm;
      }

      if(cursor->t == ADD || cursor->t == SUB){
        compconst_s* res = cc_new_node(cursor->t, &cc, &cursor->node.rm);
        last->node.rm = cursor->node.lm;
        free(cursor);

        return aux_fix_cc(res);
      }

      else {
        cc->node.lm = aux_fix_cc(cc->node.lm);
        cc->node.rm = aux_fix_cc(cc->node.rm);
        return cc;
      }

    default :
      ERR_MSG("wrong node type");
      exit(1);
  }
}

compconst_s* compconst_from_str(const char* str){
  if(!strlen(str)) return NULL;
  compconst_s * res;
  compconst_s * tmp;

  char buffer[BUFF_LEN];
  trim_spaces(str, buffer);
  tmp = aux_ccfromstr(buffer, strlen(buffer));
  res = aux_fix_cc(tmp);

  return res;
}

int compconst_eval(compconst_s* ce, double coeffs[], double* const_term){
  double coeffsl[STATS_COUNT] = {0};
  double coeffsr[STATS_COUNT] = {0};
  double ctl = 0, ctr = 0;
  switch (ce->t) {
    case DOUBLE :
      *const_term = ce->value;
      return 0;
    case LIT :
      coeffs[ce->lit] = 1.;
      return 0;
    case BRACES :
      compconst_eval(ce->braces, coeffs, const_term);
      return 0;

    case ADD :
      compconst_eval(ce->node.lm, coeffsl, &ctl);
      compconst_eval(ce->node.rm, coeffsr, &ctr);
      add_vec(STATS_COUNT, coeffsl, coeffsr, coeffs);
      *const_term = ctl + ctr;
      return 0;

    case SUB :
      compconst_eval(ce->node.lm, coeffsl, &ctl);
      compconst_eval(ce->node.rm, coeffsr, &ctr);
      scalar_mult_vec(STATS_COUNT, -1., coeffsr, coeffsr);
      add_vec(STATS_COUNT, coeffsl, coeffsr, coeffs);
      *const_term = ctl - ctr;
      return 0;

    case MULT:
      compconst_eval(ce->node.lm, coeffsl, &ctl);
      compconst_eval(ce->node.rm, coeffsr, &ctr);

      int i;
      int flagl = 0;
      int flagr = 0;

      putchar('\n');
      for(i = 0; i<STATS_COUNT; i++){
        if (coeffsl[i])
          flagl = 1;

        if (coeffsr[i])
          flagr = 1;
      }

      if (flagl & flagr){
        ERR_MSG("you shall not write non linear expressions");
        exit(1);
      }

      if(flagl)
        scalar_mult_vec(STATS_COUNT, ctr, coeffsl, coeffs);

      if(flagr)
        scalar_mult_vec(STATS_COUNT, ctl, coeffsr, coeffs);

      *const_term = ctl*ctr;
      return 0;

    default :
      ERR_MSG("wrong node type");
      exit(1);
  }
}

void printtab(int depth) {
  int i;
  for(i=0; i<depth; i++)
    putchar('\t');
}

void print_cc(compconst_s* cc, int depth){
  switch (cc->t){
    case LIT :
      printtab(depth);
      printf("%s\n", stats_symbols[cc->lit]);
      break;
    case DOUBLE :
      printtab(depth);
      printf("%f\n", cc->value);
      break;
    case BRACES :
      printtab(depth);
      printf("(\n");
      print_cc(cc->braces, depth + 1);
      printtab(depth);
      printf(")\n");
      break;
    case MULT :
      print_cc(cc->node.lm, depth + 1);
      printtab(depth);
      printf("*\n");
      print_cc(cc->node.rm, depth + 1);
      break;
    case ADD :
      print_cc(cc->node.lm, depth+1);
      printtab(depth);
      printf("+\n");
      print_cc(cc->node.rm, depth +1);
      break;
    case SUB :
      print_cc(cc->node.lm, depth+1);
      printtab(depth);
      printf("-\n");
      print_cc(cc->node.rm, depth+1);
      break;
    default:
      exit(1);
  }
}
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

    case SUB:
    case ADD:
    case MULT:
      if(temp->node.lm) free_compconst(&temp->node.lm);
      if(temp->node.rm) free_compconst(&temp->node.rm);

    default :
      break;
  }

  free(temp);
  *ce = NULL;
}
