#include "constree.h"

constree_s* new_constree(int stat, char sign, int val){
  constree_s* res = (constree_s *) malloc(sizeof(constree_s));
  res->stat = stat; res->sign = sign; res->val = val;
  res->lt = NULL; res->rt = NULL;
  return res;
}

constree_s* constree_from_str(const char* str){
  /*states
   * reading a leaf
   * reading ( 
   * reading | or &
   */
  
  if(!strlen(str)) return NULL;
  constree_s * a;
  constree_s * c;
  constree_s * b;
  char sym[3];
  char* endptr;
  int val, stat, count;

  if(str[0] == '('){
    count = 0;
    while(str[count] != ')') count ++;
    a = constree_from_str(str+1);
    if(str[count+1] == '\0') return a;

    c = constree_from_str(str+count+2);

    b = new_constree(-1, str[count+1], 1);

    b->lt = a; b->rt = c;
    return b;
  }
  else {
    sym[0] = str[0]; sym[1] = str[1]; sym[2] = '\0';
    val = (int) strtol(str+3, &endptr, 10);
    stat = symbol_to_stat(sym);

    if (stat < 0) a = NULL;
    else a = new_constree(stat, str[2], val); 

    if(endptr[0] == '\0' || endptr[0] == ')') return a;

    c = constree_from_str(endptr+1);
    b = new_constree(-1, endptr[0], -1);
    b->lt = a; b->rt = c;
    return b;
  }
}

/* TODO fix */
void cleantree(constree_s** ct){
  constree_s* tmp = *ct;
  if(!tmp) return;
  cleantree(&tmp->lt);
  cleantree(&tmp->rt);
  if(!tmp->lt && !tmp->rt && tmp->stat < 0) {free(tmp); *ct = NULL;}
  /*
  if(!tmp->lt == NULL && tmp->rt && tmp->stat < 0) {
    constree_s* tmp2 = tmp->rt;
    free(tmp);
    *ct = tmp2;
  }
  if(tmp->lt == NULL && !tmp->rt && tmp->stat < 0) {
    constree_s* tmp2 = tmp->lt;
    free(tmp);
    *ct = tmp2;
  }
  */
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

void printree_aux(constree_s* ct){
  if(ct!=NULL){
    if(ct->stat == -1){
      if(ct->val == -1){
        printree_aux(ct->lt); printf("%c", ct->sign); printree_aux(ct->rt);
      }
      if(ct->val == 1){
        printf("("); printree_aux(ct->lt); printf(")"); printree_aux(ct->rt);
      }
    }
    if(ct->stat >= 0)
      printf(" %s%c%d ", stats_names[ct->stat], ct->sign, ct->val);
  }
}

void printree(constree_s* ct){
  printree_aux(ct);
  printf("\n");
}

void free_constree(constree_s** ct){
  constree_s * tmp = *ct;
  if(!tmp) return;
  if(tmp->lt)
    free_constree(&tmp->lt);
  if(tmp->rt)
    free_constree(&tmp->rt);
  free(tmp);
  *ct = NULL;
}


