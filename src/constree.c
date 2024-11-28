#include "constree.h"
#define ERR_MSG(str) fprintf(stderr, "%d %s\n", __LINE__, (str))

constree_s* new_leaf(int stat, int sign, int val){
  constree_s * res = (constree_s *) malloc(sizeof(constree_s));
  res->t = LEAF;
  res->leaf.stat = stat;
  res->leaf.sign = sign;
  res->leaf.val = val;

  return res;
}

constree_s* new_braces(constree_s** ct){
  constree_s * res = (constree_s *) malloc(sizeof(constree_s));
  res->t = BRACES;
  res->braces = *ct;

  return res;
}

constree_s* new_node(int t, constree_s** lm, constree_s** rm){
  if(t != OR && t != AND) exit(1);
  constree_s * res = (constree_s *) malloc(sizeof(constree_s));
  res->t = t;
  res->node.lm = *lm;
  res->node.rm = *rm;

  return res;
}

constree_s* constree_from_str(const char* str) {
  if(!strlen(str)) return NULL;
  constree_s * a;
  constree_s * c;
  constree_s * b;
  constree_s * d;
  char sym[3];
  char* endptr;
  int val, stat, count, type, sign;

    ERR_MSG(str);
  if(str[0] == '('){
    count = 0;
    while(str[count] != ')') count ++;
    ERR_MSG(str+count+1);
    d = constree_from_str(str+1);
    a = new_braces(&d);
    if(str[count+1] == '\0') return a;

    c = constree_from_str(str+count+2);
    if(str[count+1] == '&') type = AND;
    else if(str[count+1] == '|') type = OR;
    else exit(1);

    b = new_node(type, &a, &c);

    return b;
  }
  else {
    sym[0] = str[0]; sym[1] = str[1]; sym[2] = '\0';
    val = (int) strtol(str+3, &endptr, 10);
    if(endptr==str+3) return NULL;

    ERR_MSG(endptr+1);
    stat = symbol_to_stat(sym);

    if(str[2] == '<') sign = -1;
    else if(str[2] == '>') sign = 1;
    else if(str[2] == '=') sign = 0;
    else exit(1);

    a = new_leaf(stat, sign, val); 

    if(endptr[0] == '\0' || endptr[0] == ')') return a;

    c = constree_from_str(endptr+1);
    if(str[2] == '&') type = AND;
    else if(str[2] == '|') type = OR;
    else exit(1);
    b = new_node(type, &a, &c);

    return b;
  }
}

int symbol_to_stat(char symbol[3]){
  if(!strcmp(symbol, "CM")) return PM;
  if(!strcmp(symbol, "CP")) return PA;
  if(!strcmp(symbol, "CV")) return VITA;
  if(!strcmp(symbol, "CW")) return SA;
  if(!strcmp(symbol, "CC")) return CHA;
  if(!strcmp(symbol, "CS")) return FOR;
  if(!strcmp(symbol, "CA")) return AGI;
  if(!strcmp(symbol, "CI")) return INT;
  if(!strcmp(symbol, "Pk")) return SET_BONUS;

  return -1;
}


void print_constree_aux(constree_s* ct){
  if(!ct) return;
  switch (ct->t) {
    case LEAF :
      if(ct->leaf.stat > 0)
        printf("%s %d %d", stats_names[ct->leaf.stat], ct->leaf.sign,
          ct->leaf.val);
      break;
      
    case BRACES :
      printf("(");
      if(ct->braces) print_constree_aux(ct->braces);
      printf(")");
      break;

    case OR :
      if(ct->node.lm) print_constree_aux(ct->node.lm);
      printf(" | ");
      if(ct->node.rm) print_constree_aux(ct->node.rm);
      break;
    case AND :
      if(ct->node.lm) print_constree_aux(ct->node.lm);
      printf(" & ");
      if(ct->node.rm) print_constree_aux(ct->node.rm);
      break;

    default :
      break;
  }
}

void print_constree(constree_s* ct){
  print_constree_aux(ct);
  printf("\n");
}

void free_constree(constree_s** ct){
  constree_s* temp = *ct;
  if(!temp) return;

  switch (temp->t) {
    case LEAF :
      break;

    case BRACES :
      if(temp->braces) free_constree(&temp->braces);
      break;

    case OR :
    case AND :
      if(temp->node.lm) free_constree(&temp->node.lm);
      if(temp->node.rm) free_constree(&temp->node.rm);
      break;

    default :
      break;
  }

  free(temp);
  *ct = NULL;
}
