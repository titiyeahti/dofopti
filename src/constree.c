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
  int braces_count;

  if(str[0] == '('){
    count = 0;
    braces_count = 1;
    while(braces_count){
      count ++;
      if(str[count] == ')') braces_count --;
      if(str[count] == '(') braces_count ++;
    }


    d = constree_from_str(str+1);
    a = new_braces(&d);
    if(str[count+1] != '|' && str[count+1] != '&') return a;
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

    stat = symbol_to_stat(sym);

    if(str[2] == '<') sign = -1;
    else if(str[2] == '>') sign = 1;
    else if(str[2] == '=') sign = 0;
    else exit(1);

    a = new_leaf(stat, sign, val); 

    if(endptr[0] != '&' && endptr[0] != '|')
      return a;

    c = constree_from_str(endptr+1);
    if(endptr[0] == '&') type = AND;
    else if(endptr[0] == '|') type = OR;
    else exit(1);
    b = new_node(type, &a, &c);

    return b;
  }
}

int eval(constree_s* ct){
  if(!ct) return 1;
  switch (ct->t){
    case LEAF :
      if (ct->leaf.stat < 0)
        return 1;
      else 
        return 0;
    case BRACES :
      return eval(ct->braces);
    case OR :
    case AND :
      return eval(ct->node.lm) || eval(ct->node.lm);
    default :
      return 1;
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

constree_s* cutting(constree_s* ct){
  constree_s *a, *b;
  if(!ct) return NULL;
  if(eval(ct)) return NULL;

  switch (ct->t) {
    case LEAF :
      return new_leaf(ct->leaf.stat, ct->leaf.sign, ct->leaf.val);
      
    case BRACES :
      a = cutting(ct->braces);
      return new_braces(&a);

    case OR :
    case AND :
      if(eval(ct->node.lm))
        return cutting(ct->node.rm);

      if(eval(ct->node.rm)) 
        return cutting(ct->node.lm);

      a = cutting(ct->node.lm); b = cutting(ct->node.rm);
      return new_node(ct->t, &a, &b);

    default :
      break;
  }
}



void print_constree_aux(constree_s* ct){
  if(!ct) return;
  if(eval(ct)) return;

  switch (ct->t) {
    case LEAF :
      char csig;
      csig = ct->leaf.sign == -1 ? '<' : 
        (ct->leaf.sign == 1 ? '>' : '=');
      printf("%s %c %d", stats_names[ct->leaf.stat], csig, ct->leaf.val);
      break;
      
    case BRACES :
      printf("(");
      print_constree_aux(ct->braces);
      printf(")");
      break;

    case OR :
    case AND :
      if(eval(ct->node.lm)) {
        print_constree_aux(ct->node.rm);
        break;
      }
      if(eval(ct->node.rm)) {
        print_constree_aux(ct->node.lm);
        break;
      }

      print_constree_aux(ct->node.lm);
      printf(ct->t == OR ? " | " : " & ");
      print_constree_aux(ct->node.rm);
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
