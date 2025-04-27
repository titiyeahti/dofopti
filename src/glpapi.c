#include"glpapi.h"

#define DEF(e, str, s) str
const char* const stats_names[] = {
#include "stats.conf"
};
#undef DEF

#define DEF(e, c, s, cs) s
const char* const slots_names[] = {
#include "slots.conf"
};
#undef DEF

double matrix_pptac[STATS_COUNT*STATS_COUNT] = {0.};


int init(){
  fill_pptac();
  return 0;
}

int fill_pptac(){
  int i;
  for(i = 0; i<STATS_COUNT; i++) matrix_pptac[i*STATS_COUNT+i] = 1.;
  matrix_pptac[RES_PA+STATS_COUNT*SA] = .1;
  matrix_pptac[RET_PA+STATS_COUNT*SA] = .1;

  matrix_pptac[RES_PM+STATS_COUNT*SA] = .1;
  matrix_pptac[RET_PM+STATS_COUNT*SA] = .1;

  matrix_pptac[INI+STATS_COUNT*AGI] = 1.;
  matrix_pptac[INI+STATS_COUNT*CHA] = 1.;
  matrix_pptac[INI+STATS_COUNT*INT] = 1.;
  matrix_pptac[INI+STATS_COUNT*FOR] = 1.;

  matrix_pptac[TAC+STATS_COUNT*AGI] = .1;
  matrix_pptac[FUI+STATS_COUNT*AGI] = .1;

  matrix_pptac[PP+STATS_COUNT*CHA] = .1;

  matrix_pptac[PODS+STATS_COUNT*FOR] = 5.;

  return 0;
}

int select_count_items(sqlite3* db, int level){
  int ret;
  int nb;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, 
      "SELECT count(distinct id) FROM items where level <= ?;", -1, &stmt, NULL);
  ret = sqlite3_bind_int(stmt, 1, level);

  SQL_CHECK_ERRORS(ret)
    ret = sqlite3_step(stmt);
  SQL_CHECK_ERRORS(ret)
    nb = sqlite3_column_int(stmt, 0);
  ret = sqlite3_finalize(stmt);
  return nb;
}

int select_count_panos(sqlite3* db){
  int ret;
  int nb;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, "SELECT count(distinct itemSetId) FROM items where itemSetId > -1;", -1, &stmt, NULL);
  ret = sqlite3_step(stmt);
  SQL_CHECK_ERRORS(ret)
    nb = sqlite3_column_int(stmt, 0);
  ret = sqlite3_finalize(stmt);
  return nb;
}

int select_count_bonuses(sqlite3* db){
  int ret;
  int nb;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, 
      "select sum(b) from (select max(nbItems) as b from item_sets join set_bonuses on setItemId = id group by setItemId);",
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);
  SQL_CHECK_ERRORS(ret)
    nb = sqlite3_column_int(stmt, 0);
  ret = sqlite3_finalize(stmt);
  return nb;
}

int get_items_stats(sqlite3* db, statline_s* items_data, int level){
  int ret, i;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, 
      "select items.id, statCode, maxval, namefr, itemSetId, slotCode from items "
      "left join item_stats on id = itemId "
      "where level <= ? order by items.id;",
      -1, &stmt, NULL);
  SQL_CHECK_ERRORS(ret);
  ret = sqlite3_bind_int(stmt, 1, level);
  SQL_CHECK_ERRORS(ret);
  int last_id = -1;
  int id = -1;
  /*WARNING*/
  int count = 0;
  while((ret = sqlite3_step(stmt)) == SQLITE_ROW){
    id = sqlite3_column_int(stmt, 0);
    if (id != last_id){
      count++;
      for(i = 0; i<STATS_COUNT; i++) items_data[count].stats[i] = 0; 
      items_data[count].id_pano = sqlite3_column_int(stmt, 4);
      items_data[count].id = id;
      strcpy(items_data[count].name, (const char*) 
          sqlite3_column_text(stmt, 3));
      items_data[count].slot_code = sqlite3_column_int(stmt, 5);
      last_id = id;
    }

    items_data[count].stats[sqlite3_column_int(stmt, 1)] = 
      sqlite3_column_int(stmt, 2);
  }
  SQL_CHECK_ERRORS(ret)

    ret = sqlite3_finalize(stmt);

  return ret;
}

int get_bonuses(sqlite3* db, statline_s* bonuses_data){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, 
      "select setItemId, namefr, nbItems, statCode, val from set_bonuses join item_sets on id = setItemId order by setItemId, nbItems;", 
      -1, &stmt, NULL);

  int nb_slots = 0;
  int last_nb_slots = 0;
  int last_id = -1;
  int id = -1;
  int count = -1;
  int j, i;
  while((ret=sqlite3_step(stmt)) == SQLITE_ROW){
    id = sqlite3_column_int(stmt, 0);
    nb_slots = sqlite3_column_int(stmt, 2);
    if(id != last_id){
      for(i=0; i<nb_slots; i++){
        count++;
        for(j = 0; j<STATS_COUNT; j++) bonuses_data[count].stats[j] = 0; 
        sprintf(bonuses_data[count].name, "%s#%d",
            sqlite3_column_text(stmt, 1),
            i+1);
        bonuses_data[count].id_pano = id;
        bonuses_data[count].stats[SET_BONUS] = i;
      }

      last_id = id;
      last_nb_slots = nb_slots;
    }

    if(nb_slots!=last_nb_slots){
      count++;
      for(j = 0; j<STATS_COUNT; j++) bonuses_data[count].stats[j] = 0; 
      sprintf(bonuses_data[count].name, "%s#%d",
          sqlite3_column_text(stmt, 1),
          nb_slots);
      bonuses_data[count].id_pano = id;
      bonuses_data[count].stats[SET_BONUS] = nb_slots - 1;
      last_nb_slots = nb_slots;
    }

    bonuses_data[count].stats[sqlite3_column_int(stmt, 3)] = 
      sqlite3_column_int(stmt, 4);
  }
  SQL_CHECK_ERRORS(ret)

    ret = sqlite3_finalize(stmt);
  return ret;
}

/* malloc */
int get_panos_info(sqlite3* db, pbdata_s* res){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db,
      "select count(distinct items.id), items.itemSetId, max(nbItems), item_sets.namefr "
      "from items join set_bonuses on set_bonuses.setItemId = items.itemSetId "
      "join item_sets on item_sets.id = items.itemSetId group by items.itemSetId;",
      -1, &stmt, NULL);

  int count = 0;
  while(sqlite3_step(stmt) == SQLITE_ROW){
    res->panos[count].id_pano = sqlite3_column_int(stmt, 1);
    res->panos[count].size = sqlite3_column_int(stmt, 0);
    res->panos[count].maxbonuses = sqlite3_column_int(stmt, 2);
    strcpy(res->panos[count].name, (const char*) sqlite3_column_text(stmt, 3));
    count ++;
  }
  SQL_CHECK_ERRORS(ret)

    ret = sqlite3_finalize(stmt);
  return ret;
}

/*TODO BIG WARNING
 * maybe problematic if items are not present*/
int fill_panos_id(pbdata_s* res){
  int i, j, k;
  for(i = 0; i < res->nb_panos; i++){
    /*items*/
    /*for(j=0; j<20; j++) res->panos[i].ids[j] = 0;*/
    k = 0;
    for(j = 1; j < res->nb_items+1; j++){
      if(res->items_data[j].id_pano == res->panos[i].id_pano){
        res->panos[i].ids[k+1] = j;
        k++;
        if(k==res->panos[i].size)
          break;
      }
    }

    if(j==res->nb_items+1) res->panos[i].size = k;

    /* binary search maybe*/
    k = 0;
    for(j = 0; j < res->nb_bonuses; j++){
      if(res->bonuses_data[j].id_pano == res->panos[i].id_pano){
        res->panos[i].ids[k+1+res->panos[i].size] = j + res->nb_items + 1;
        k++;
        if(k==res->panos[i].maxbonuses)
          break;
      }
    }
  }

  return 0;
}

int new_pbdata(sqlite3* db, pbdata_s* res, stat_vect base_stats,
    int targeted_slots[SLOT_COUNT], int level){
  int nb_items, nb_bonuses, nb_panos;
  int ret, i;

  if(targeted_slots) 
    memcpy(res->targeted_slots, targeted_slots, 
        sizeof(int)*SLOT_COUNT);
  else{
    for(i=0; i<SLOT_COUNT; i++)
      res->targeted_slots[i] = 1;

    res->targeted_slots[SLOT_RING] = 2;
    res->targeted_slots[SLOT_DOFUS] = 6;
  }

  if(level < 1 || level > 200)
    res->level = 200;
  else
    res->level = level;

  for(i=0; i<STATS_COUNT; i++)
    res->base_stats[i] = 0;

  res->base_stats[PA] = res->level >= 100 ? 7 : 6;
  res->base_stats[PM] = 3;
  res->base_stats[PODS] = 1000 + (res->level - 1) * 5;
  res->base_stats[VITA] = 50 + 5*res->level;
  res->base_stats[INVO] = 1;
  res->base_stats[PP] = 100;

  if (base_stats) {
    for(i=0; i<STATS_COUNT; i++)
      res->base_stats[i] += base_stats[i];
  }

  nb_items = select_count_items(db, res->level);
  nb_panos = select_count_panos(db);
  nb_bonuses = select_count_bonuses(db);
  res->nb_items = nb_items;
  res->nb_panos = nb_panos;
  res->nb_bonuses = nb_bonuses;

  res->items_data = (statline_s*) malloc((nb_items+1)*sizeof(statline_s));

  ret = get_items_stats(db, res->items_data, res->level);

  res->bonuses_data = (statline_s*) malloc(nb_bonuses*sizeof(statline_s));

  ret = get_bonuses(db, res->bonuses_data);

  res->panos = (pids_s*) malloc(nb_panos*sizeof(pids_s));

  ret = get_panos_info(db, res);

  fill_panos_id(res);

  return ret;
}

void free_pbdata(pbdata_s* pbd){
  free(pbd->panos);
  free(pbd->items_data);
  free(pbd->bonuses_data);
}

void print_statline(statline_s* sl){
  printf("%s: ", sl->name);
  int i;
  for(i = 0; i < STATS_COUNT; i++)
    if(sl->stats[i])
      printf("%s [%d] ", stats_names[i], sl->stats[i]);

  printf("\n");
}

void print_pids(pids_s* pi){
  int i;
  printf("%d: ( ", pi->id_pano);
  for(i = 0; i < pi->size; i++)
    printf("%d ", pi->ids[i+1]);
  printf(")(");
  for(i = pi->size; i < pi->size + pi->maxbonuses; i++)
    printf("%d ", pi->ids[i+1]);

  printf(")\n");
}

void print_pbdata(pbdata_s* pbd){
  int i;
  for(i=0; i<SLOT_COUNT; i++)
    printf("%s [%d] ", slots_names[i], pbd->targeted_slots[i]);

  printf("\nLEVEL : %d\nBASE_STATS : \n", pbd->level);
  for(i=0; i<STATS_COUNT; i++)
    if(pbd->base_stats[i]) printf("%s [%d] ", stats_names[i], pbd->base_stats[i]);

  printf("\nITEMS [%d]\n", pbd->nb_items);
  for(i = 1; i < pbd->nb_items+1; i++){
    printf("%d\t", i);
    print_statline(&(pbd->items_data[i]));
  }

  printf("\nBONUSES [%d]\n", pbd->nb_bonuses);
  for(i = 0; i < pbd->nb_bonuses; i++){
    printf("%d\t", i+pbd->nb_items+1);
    print_statline(&(pbd->bonuses_data[i]));
  }

  printf("\nPANOS IDS [%d]\n", pbd->nb_panos);
  for(i = 0; i < pbd->nb_panos; i++){
    print_pids(&(pbd->panos[i]));
  }
}

void print_vec(size_t n, double vec[]){
  size_t j;
  for(j=0; j<n; j++){
    printf("%f\t", vec[j]);
  }
  printf("\n");
}

void add_vec(size_t n, double v1[], double v2[], double output[]){
  size_t i;
  for(i=0; i<n; i++)
    output[i] = v1[i]+v2[i];
}

void scalar_mult_vec(size_t n, double lambda, double input[], double output[]){
  size_t  i;
  for(i=0; i<n; i++)
    output[i] = lambda*input[i];
}

void print_matrix(size_t n, size_t m, double matrix[]){
  size_t i, j;
  for(i=0; i<n; i++){
    for(j=0; j<m; j++){
      if(matrix[i*m + j] > .001 || -matrix[i*m+j] > .001)
        printf("%ld, %ld -> %f\t",i, j, matrix[i*m+j]);
    }
    printf("\n");
  }
}

int mat_times_vec(size_t n, size_t m, double matrix[], 
    double input[], double output[]){
  size_t i, j;

  double cur;

  for(i=0; i<n; i++){
    cur = 0;
    for(j=0; j<m; j++){
      cur += input[j]*matrix[i*m+j];
    }
    output[i] = cur;
  }

  return 0;
}

int vec_times_mat(size_t n, size_t m, double matrix[],
    double input[], double output[]){
  size_t i, j;

  double cur;

  for(i=0; i<m; i++){
    cur = 0;
    for(j=0; j<n; j++){
      cur += input[j]*matrix[j*m+i];
    }
    output[i] = cur;
  }

  return 0;
}

int stat_to_basis(size_t n, size_t m, double matrix[],
    double input[], double output[]){
  /*intermediate value*/
  double oo[n];
  mat_times_vec(m, m, matrix_pptac, input, oo);
  mat_times_vec(n, m, matrix, oo, output);
  return 0;
}

int basis_to_stat(size_t n, size_t m, double matrix[],
    double input[], double output[]){
  double oo[m];
  vec_times_mat(n, m, matrix, input, oo);
  vec_times_mat(m, m, matrix_pptac, oo, output);
  return 0;
}

int linprob_items_variables(linprob_s* res, pbdata_s* pbd){
  int i;
  glp_add_cols(res->pb, pbd->nb_items);
  /*adding items variables*/
  for(i=1; i<pbd->nb_items+1; i++){
    glp_set_col_name(res->pb, i, pbd->items_data[i].name);
    glp_set_col_kind(res->pb, i, GLP_BV);
    /*off set rings*/
    if(pbd->items_data[i].slot_code == SLOT_RING &&
        pbd->items_data[i].id_pano == -1){
      glp_set_col_kind(res->pb, i, GLP_IV);
      glp_set_col_bnds(res->pb, i, GLP_DB, 0., 2.);
    }
  }

  return 0;
}

int linprob_bonuses_variables(linprob_s* res, pbdata_s* pbd){
  int i;
  glp_add_cols(res->pb, pbd->nb_bonuses);
  /*adding bonuses variables*/
  for(i=0; i < pbd->nb_bonuses; i++){
    glp_set_col_name(res->pb, i+1+pbd->nb_items, pbd->bonuses_data[i].name);
    glp_set_col_kind(res->pb, i+1+pbd->nb_items, GLP_BV);
  }
  return 0;
}

int linprob_slots_constraints(linprob_s* res, pbdata_s* pbd){
  /*slot constraints*/
  int i, j, count;
  double vec[pbd->nb_items+1];
  for(i=0; i<pbd->nb_items; i++) vec[i+1] = 1.;

  glp_add_rows(res->pb, SLOT_COUNT);
  int ids[pbd->nb_items+1];
  for(i=0; i<SLOT_COUNT; i++){
    count = 1;
    for(j=1; j<pbd->nb_items+1; j++){
      if(pbd->items_data[j].slot_code == i){
        ids[count] = j;
        count ++;
      }
    }

    glp_set_row_name(res->pb, i+1, slots_names[i]);
    glp_set_row_bnds(res->pb, i+1, GLP_UP, 0., (double) pbd->targeted_slots[i]);
    glp_set_mat_row(res->pb, i+1, count-1, ids, vec);
  }

  glp_set_row_bnds(res->pb, SLOT_RING +1, GLP_UP, 0, 
      (double) pbd->targeted_slots[SLOT_RING]);

  glp_set_row_bnds(res->pb, SLOT_DOFUS +1, GLP_UP, 0, 
      (double) pbd->targeted_slots[SLOT_DOFUS]);

  glp_set_row_bnds(res->pb, SLOT_PRISMA +1, GLP_UP, 0, 
      (double) pbd->targeted_slots[SLOT_PRISMA]);

  glp_add_rows(res->pb, 1);
  count = 1;
  for(j=1; j<pbd->nb_items+1; j++){
    int code = pbd->items_data[j].slot_code;
    if(code == SLOT_DOFUS || code == SLOT_PRISMA){
      ids[count] = j;
      count ++;
    }
  }

  i = glp_get_num_rows(res->pb);
  glp_set_row_name(res->pb, i, "DOFUS + PRISMA");
  glp_set_row_bnds(res->pb, i, GLP_UP, 0, 6);
  glp_set_mat_row(res->pb, i, count-1, ids, vec);

  return 0;
}

int add_pano_constraints(glp_prob* pb, pids_s pano){
  short_word glp_name;
  int rownum = glp_get_num_rows(pb);
  int j;
  glp_add_rows(pb, 2);
  double coeffs[pano.size + pano.maxbonuses + 1];
  for(j=0; j<pano.size; j++) coeffs[j+1] = 1.;
  for(j=0; j<pano.maxbonuses; j++) coeffs[j+1+pano.size] = (double) -(j+1);
  sprintf(glp_name, "%s-a", pano.name);
  glp_set_row_name(pb, rownum+1, glp_name);
  glp_set_row_bnds(pb, rownum+1, GLP_FX, 0., 0.);
  glp_set_mat_row(pb, rownum+1, pano.size+pano.maxbonuses, pano.ids, coeffs);

  sprintf(glp_name, "%s-b", pano.name);
  glp_set_row_name(pb, rownum+1+1, glp_name);
  glp_set_row_bnds(pb, rownum+1+1, GLP_UP, 0., 1.);
  glp_set_mat_row(pb, rownum+1+1, pano.maxbonuses, 
      &pano.ids[pano.size], coeffs);

  return 0;
}

int stats_repartition(glp_prob* pb, int level){
  int first_col, i, j, id, rownum;
  int ids[NB_COLS_FOR_STATS+1];
  double val[NB_COLS_FOR_STATS+1];
  int stat_rep[NB_ELEMS] = {AGI, CHA, INT, FOR};
  short_word name;

  first_col = glp_add_cols(pb, NB_COLS_FOR_STATS);
  id = first_col;
  for(i=0; i<NB_ELEMS; i++)
    for(j=0; j<NB_COLS_PER_ELEM; j++){
      sprintf(name, "%s-%d", stats_names[stat_rep[i]], j+1);
      glp_set_col_name(pb, id, name);
      glp_set_col_bnds(pb, id, GLP_DB, 0., 100.);
      glp_set_col_kind(pb, id, GLP_CV);
      ids[id - first_col + 1] = id;
      val[id - first_col + 1] = (double) (j + 1);
      id ++;
    }

  glp_set_col_name(pb, id, stats_names[VITA]);
  glp_set_col_bnds(pb, id, GLP_LO, 0., 0.);
  glp_set_col_kind(pb, id, GLP_CV);
  ids[id - first_col + 1] = id;
  val[id - first_col + 1] = 1.;

  id++;

  glp_set_col_name(pb, id, stats_names[SA]);
  glp_set_col_bnds(pb, id, GLP_LO, 0., 0.);
  glp_set_col_kind(pb, id, GLP_CV);
  ids[id - first_col + 1] = id;
  val[id - first_col + 1] = 3.;

  rownum = glp_add_rows(pb, 1);
  glp_set_mat_row(pb, rownum, NB_COLS_FOR_STATS, ids, val);
  glp_set_row_name(pb, rownum, "carac distribution");
  glp_set_row_bnds(pb, rownum, GLP_DB, 0., (level-1) * 5.);

  return 0;
}

linprob_s* new_linprob(pbdata_s* pbd){
  int i, j, items_bonuses;
  items_bonuses = pbd->nb_items + pbd->nb_bonuses;

  linprob_s* res = malloc(sizeof(linprob_s));
  /* + 1 is for bases stats */
  res->n = items_bonuses + NB_COLS_FOR_STATS + 1;
  res->m = STATS_COUNT;
  res->pb = glp_create_prob();
  glp_create_index(res->pb);
  res->matrix = malloc(sizeof(double) * res->n  * res->m);

  for(j=0; j<res->m; j++)
    res->matrix[j] = pbd->base_stats[j];

  for(i=0; i<pbd->nb_items; i++)
    for(j=0; j<res->m; j++)
      res->matrix[(i+1)*res->m + j] = pbd->items_data[i+1].stats[j];

  for(i=0; i<pbd->nb_bonuses; i++)
    for(j=0; j<res->m; j++)
      res->matrix[(i+pbd->nb_items+1)*res->m + j] = pbd->bonuses_data[i].stats[j];

  for(i=0; i<NB_COLS_FOR_STATS; i++)
    for(j=0; j<res->m; j++)
      res->matrix[(i+items_bonuses+1)*res->m + j] = 0.;

  int stat_rep[NB_ELEMS] = {AGI, CHA, INT, FOR};
  /* elementary stats */
  for(i=0; i<NB_ELEMS; i++)
    for(j=0; j<NB_COLS_PER_ELEM; j++)
      res->matrix[(i*NB_COLS_PER_ELEM+j+items_bonuses+1)*res->m + 
        stat_rep[i]] = 1.;

  res->matrix[(NB_COLS_PER_ELEM*NB_ELEMS+items_bonuses+1)*res->m +
    VITA] = 1.;

  res->matrix[(NB_COLS_PER_ELEM*NB_ELEMS+items_bonuses+1+1)*res->m +
    SA] = 1.;

  glp_set_prob_name(res->pb, "pb_name");
  glp_set_obj_name(res->pb, "obj_func");
  glp_set_obj_dir(res->pb, GLP_MAX);

  linprob_items_variables(res, pbd);
  linprob_bonuses_variables(res, pbd);
  linprob_slots_constraints(res, pbd);
  /*sets (panos) constraints*/
  for(i=0; i<pbd->nb_panos; i++){
    if(pbd->panos[i].maxbonuses > 9) {
      printf("%s, %d, %d\n", pbd->panos[i].name, 
          pbd->panos[i].size, pbd->panos[i].maxbonuses);
      continue;
    }
    add_pano_constraints(res->pb, pbd->panos[i]);
  }

  stats_repartition(res->pb, pbd->level);

  return res;
}

int set_obj_coeff(linprob_s* lp, double coeffs[]){
  double output[lp->n];
  stat_to_basis(lp->n, lp->m, lp->matrix, coeffs, output);
  int i;

  for(i=0; i<lp->n; i++) glp_set_obj_coef(lp->pb, i, output[i]);

  return 0;
}

/*WARNING
 * maybe issues with "n" since there is base stats now*/
int const_linear(linprob_s* lp, double coeffs[], const char* name, double* const_term){
  int i, ret, rownum;
  double output[lp->n];
  int ids[lp->n];
  for(i = 0; i<lp->n; i++) ids[i] = i;

  stat_to_basis(lp->n, lp->m, lp->matrix, coeffs, output);
  rownum = glp_add_rows(lp->pb, 1);

  glp_set_mat_row(lp->pb, rownum, lp->n-1, ids, output);

  *const_term = output[0];

  if(name)
    glp_set_row_name(lp->pb, rownum, name);

  return rownum;
}

int const_multi_simple_constraints(linprob_s* lp, double bounds[], int sign[]){
  double coeffs[STATS_COUNT] = {0};
  int i;
  for(i=0; i<lp->m; i++){
    if(bounds[i]>0.1 || -bounds[i]>0.1){
      coeffs[i] = 1.;
      if(sign[i] < 0){
        const_linear_lower(lp, coeffs, bounds[i], stats_names[i]);
      }
      else if (sign[i] > 0){
        const_linear_upper(lp, coeffs, bounds[i], stats_names[i]);
      }
      else{
        const_linear_fix(lp, coeffs, bounds[i], stats_names[i]);
      }
      coeffs[i] = 0.;
    }
  }

  return 0; 
}

int const_linear_upper(linprob_s* lp, double coeffs[],
    double bound, const char* name){
  double const_term;
  int ret = const_linear(lp, coeffs, name, &const_term);
  glp_set_row_bnds(lp->pb, ret, GLP_UP, bound, bound - const_term);
  return ret;
}

int const_linear_lower(linprob_s* lp, double coeffs[],
    double bound, const char* name){
  double const_term;
  int ret = const_linear(lp, coeffs, name, &const_term);
  glp_set_row_bnds(lp->pb, ret, GLP_LO, bound - const_term, bound - const_term);
  return ret;
}

int const_linear_fix(linprob_s* lp, double coeffs[],
    double bound, const char* name){
  double const_term;
  int ret = const_linear(lp, coeffs, name, &const_term);
  glp_set_row_bnds(lp->pb, ret, GLP_FX, bound - const_term, bound - const_term);
  return ret;
}

int solve_linprob(linprob_s* lp){
  // config parm
  glp_iocp parm;
  glp_init_iocp(&parm);
  parm.presolve = GLP_ON;

  // solve
  glp_intopt(lp->pb, &parm);
  return 0;
}

/*Rewamp the function to write into a FILE* */
void print_linsol(linprob_s* lp, pbdata_s* pbd, FILE* stream){
  int i;
  double vec[lp->n];
  double stats[STATS_COUNT];
  vec[0] = 0;
  for(i=1; i<lp->n ; i++){

    double val = glp_mip_col_val(lp->pb, i);
    vec[i] = val;
    if(val>0.5){
      if (i < pbd->nb_items+1)
        fprintf(stream, "%s : %s [%d]\n", slots_names[pbd->items_data[i].slot_code], 
          glp_get_col_name(lp->pb, i), (int) val);
      else if (i >= pbd->nb_items + pbd->nb_bonuses + 1)
        fprintf(stream, "%s [%d]\n", glp_get_col_name(lp->pb, i), (int) val);
      else 
        fprintf(stream, "%s\n", glp_get_col_name(lp->pb, i));
    }
  }
  fprintf(stream, "\n");

  basis_to_stat(lp->n, lp->m, lp->matrix, vec, stats);
  for(i = 0; i < STATS_COUNT; i++){
    fprintf(stream, "%s [%d]%c", stats_names[i], 
        pbd->base_stats[i]+(int)stats[i], (i+1) % 5 ? '\t' : '\n');
  }
  fprintf(stream, "\n");
}

int const_lock_in_item(linprob_s* lp, const char* name){
  int numcol = glp_find_col(lp->pb, name);
  if(numcol)
    glp_set_col_bnds(lp->pb, numcol, GLP_FX, 1., 1.);

  return numcol;
}

int const_lock_out_item(linprob_s* lp, const char* name){
  int numcol = glp_find_col(lp->pb, name);
  if(numcol)
    glp_set_col_bnds(lp->pb, numcol, GLP_FX, 0., 0.);

  return numcol;
}

void free_linprob(linprob_s* lp){
  glp_delete_prob(lp->pb);
  free(lp->matrix);

  free(lp);
}
