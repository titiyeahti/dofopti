#include "constree.h"

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

    if(str[2] == '>') sign = -1;
    else if(str[2] == '<') sign = 1;
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

  return NULL;
}

int fill_pbd_constraints(sqlite3* db, pbdata_s* pbd){
  int ret, i;
  sqlite3_stmt* stmt;
  constree_s* tempct;

  pbd->items_consts = (constree_s**) 
    malloc((pbd->nb_items+1)*sizeof(constree_s*));

  ret = sqlite3_prepare_v2(db, 
      "SELECT id, criteria FROM items where level <= ? order by id;", -1, 
      &stmt, NULL);
  ret = sqlite3_bind_int(stmt, 1, pbd->level);

  i = 1;
  while((ret=sqlite3_step(stmt)) == SQLITE_ROW){
    const char *str = (const char*) sqlite3_column_text(stmt, 1);
    tempct = constree_from_str(str);
    pbd->items_consts[i] = cutting(tempct);
    free_constree(&tempct);
    i++;
  }

  sqlite3_finalize(stmt);

  return 0;
}

void free_pbd_constraints(pbdata_s* pbd){
  int i;

  for(i=1; i<pbd->nb_items+1; i++)
    free_constree(&(pbd->items_consts[i]));
  free(pbd->items_consts);
}

void print_constree_aux(constree_s* ct){
  if(!ct) return;
  if(eval(ct)) return;

  switch (ct->t) {
    case LEAF :
      char csig;
      csig = ct->leaf.sign == -1 ? '>' : 
        (ct->leaf.sign == 1 ? '<' : '=');
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

int compute_constraint(linprob_s* lp, int item_id, int sign,
    int stat, double bnd, int yids[], int nb_ors){

  if(sign == 0) {
    fprintf(stderr, "illegal sign for item constraint, ignored\n");
    return 1;
  }

  int i;
  int rownum;
  double vec[lp->m];
  double output[lp->n];
  double const_term;
  for(i=0; i<lp->m; i++)
    vec[i] = 0.;

  vec[stat] = 1.;

  stat_to_basis(lp->n, lp->m, lp->matrix, vec, output);

  /* please mind the gap between the train and the platform*/
  const_term = bnd - output[0];
  int final_ids[lp->n+nb_ors];
  double final_coeffs[lp->n+nb_ors];

  for(i=0; i<lp->n; i++){
    final_ids[i] = i;
    final_coeffs[i] = output[i];
  }

  for(i=0; i<nb_ors; i++){
    final_ids[i+lp->n] = yids[i];
    final_coeffs[i+lp->n] = sign * BIG_M;
  }


  final_coeffs[item_id] += sign * BIG_M;

  short_word name;
  sprintf(name, "%d-%s", item_id, stats_names[stat]);
  rownum = glp_add_rows(lp->pb, 1);
  glp_set_row_name(lp->pb, rownum, name);
  glp_set_mat_row(lp->pb, rownum, lp->n+nb_ors-1, final_ids, final_coeffs);

  /* DO NOT WORK WITH FX_VARIABLES*/
  glp_set_row_bnds(lp->pb, rownum, 
      sign < 0 ? GLP_LO : (sign > 0 ? GLP_UP : GLP_FX), 
      const_term, const_term);

  return 0;
}

void add_constraint_aux(int item_id, int yids[], int nb_ors,
    linprob_s* lp, constree_s* ct){
  if(!ct) return ;
  
  switch (ct->t) {
    case LEAF :
      double bnd = (double) (ct->leaf.val - ct->leaf.sign) + 
        (double) ct->leaf.sign * 
        (double) (1+nb_ors) * BIG_M;
      
      compute_constraint(lp, item_id, ct->leaf.sign, ct->leaf.stat, 
          bnd, yids, nb_ors);
      break;
      
    case BRACES :
      add_constraint_aux(item_id, yids, nb_ors, lp, ct->braces);
      break;

    case AND :
      add_constraint_aux(item_id, yids, nb_ors, lp, ct->node.lm);
      add_constraint_aux(item_id, yids, nb_ors, lp, ct->node.rm);
      break;

    case OR :
      if(nb_ors > MAX_DEPTH-1)
        exit(1);

      int col_id = glp_add_cols(lp->pb, 2);
      short_word name;
      sprintf(name, "y-%d-%d", item_id, 2*nb_ors);
      glp_set_col_name(lp->pb, col_id, name);
      glp_set_col_kind(lp->pb, col_id, GLP_BV);

      sprintf(name, "y-%d-%d", item_id, 2*nb_ors+1);
      glp_set_col_name(lp->pb, col_id+1, name);
      glp_set_col_kind(lp->pb, col_id+1, GLP_BV);

      int cur_ids[4];
      double cur_coeffs[4];
      cur_ids[1] = col_id; cur_ids[2] = col_id+1;
      cur_coeffs[1] = 1.; cur_coeffs[2] = 1.;

      int rownum = glp_add_rows(lp->pb, 1);
      sprintf(name, "y-%d-%d[+1]", item_id, 2*nb_ors);
      glp_set_row_name(lp->pb, rownum, name);
      glp_set_mat_row(lp->pb, rownum, 2, cur_ids, cur_coeffs);
      glp_set_row_bnds(lp->pb, rownum, GLP_FX, 1., 1.);

      yids[nb_ors] = col_id;
      add_constraint_aux(item_id, yids, nb_ors+1, lp, ct->node.lm);
      yids[nb_ors] = col_id+1;
      add_constraint_aux(item_id, yids, nb_ors+1, lp, ct->node.rm);

      break;

    default :
      break;

  }
}

int add_item_constraint(int item_id, linprob_s* lp, pbdata_s* pbd){
  int yids[MAX_DEPTH];
  int nb_ors = 0;

  add_constraint_aux(item_id, yids, nb_ors, lp, pbd->items_consts[item_id]);

  return 0;
}


int add_all_items_constraints(linprob_s* lp, pbdata_s* pbd){
  int i;
  for(i=1; i<pbd->nb_items+1; i++){
    if(pbd->items_consts[i])
      add_item_constraint(i, lp, pbd);
  }

  return 0;
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
