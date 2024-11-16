#include"glpapi.h"

#define DEF(e, s) s
const char* const stats_names[] = {
#include "stats.conf"
};
#undef DEF

#define DEF(e, c, s, cs) c
const int slots_codes[] = {
#include "slots.conf"
};
#undef DEF

#define DEF(e, c, s, cs) s
const char* const slots_names[] = {
#include "slots.conf"
};
#undef DEF

#define ERR_MSG() printf("%d\n", __LINE__)

int select_count_items(sqlite3* db){
  int ret;
  int nb;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, "SELECT count(distinct id) FROM items;", -1, &stmt, NULL);
  ret = sqlite3_step(stmt);
  nb = sqlite3_column_int(stmt, 0);
  ret = sqlite3_finalize(stmt);
  return nb;
}

int select_count_panos(sqlite3* db){
  int ret;
  int nb;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, "SELECT count(distinct id_pano) FROM items;", -1, &stmt, NULL);
  ret = sqlite3_step(stmt);
  nb = sqlite3_column_int(stmt, 0);
  ret = sqlite3_finalize(stmt);
  return nb;
}

int select_count_bonuses(sqlite3* db){
  int ret;
  int nb;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, 
      "select sum(b) from (select max(nb_items) as b from panos join bonuses on bonuses.id_pano = panos.id group by id_pano);",
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);
  nb = sqlite3_column_int(stmt, 0);
  ret = sqlite3_finalize(stmt);
  return nb;
}

int get_items_stats(sqlite3* db, statline_s* items_data){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, 
      "select items.id, stat_code, maxval, name, id_pano, slot_code from items "
      "left join stats on id = id_item order by id_item;",
      -1, &stmt, NULL);

  int last_id = -1;
  int id = -1;
  /*WARNING*/
  int count = 0;
  while(sqlite3_step(stmt) == SQLITE_ROW){
    id = sqlite3_column_int(stmt, 0);
    if (id != last_id){
      count++;
      items_data[count].id_pano = sqlite3_column_int(stmt, 4);
      strcpy(items_data[count].name, (const char*) 
          sqlite3_column_text(stmt, 3));
      items_data[count].slot_code = sqlite3_column_int(stmt, 5);
      last_id = id;
    }

    items_data[count].stats[sqlite3_column_int(stmt, 1)] = 
      sqlite3_column_int(stmt, 2);
  }

  ret = sqlite3_finalize(stmt);

  return ret;
}

int get_bonuses(sqlite3* db, statline_s* bonuses_data){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db, 
      "select id_pano, name, nb_items, stat_code, value from bonuses join panos on panos.id = id_pano order by id_pano, nb_items;", 
      -1, &stmt, NULL);

  int nb_slots = 0;
  int last_nb_slots = 0;
  int last_id = -1;
  int id = -1;
  int count = -1;
  while(sqlite3_step(stmt) == SQLITE_ROW){
    id = sqlite3_column_int(stmt, 0);
    nb_slots = sqlite3_column_int(stmt, 2);
    if(id != last_id){
      int i;
      for(i=0; i<nb_slots; i++){
        count++;
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

  ret = sqlite3_finalize(stmt);
  return ret;
}

/* malloc */
int get_panos_info(sqlite3* db, pbdata_s* res){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare_v2(db,
      "select count(distinct items.id), items.id_pano, max(nb_items), panos.name from "
      "items join bonuses on bonuses.id_pano = items.id_pano "
      "join panos on panos.id = items.id_pano group by items.id_pano;",
      -1, &stmt, NULL);

  int count = 0;
  while(sqlite3_step(stmt) == SQLITE_ROW){
    res->panos[count].id_pano = sqlite3_column_int(stmt, 1);
    res->panos[count].size = sqlite3_column_int(stmt, 0);
    res->panos[count].maxbonuses = sqlite3_column_int(stmt, 2);
    strcpy(res->panos[count].name, (const char*) sqlite3_column_text(stmt, 3));

    res->panos[count].ids = 
      (int*) malloc(sizeof(int)*(res->panos[count].size + 
            res->panos[count].maxbonuses)+1);
    count ++;
  }

  ret = sqlite3_finalize(stmt);
  return ret;
}

int fill_panos_id(pbdata_s* res){
  int i, j, k;
  for(i = 0; i < res->nb_panos; i++){
    /*items*/
    k = 0;
    for(j = 1; j < res->nb_items+1; j++)
      if(res->items_data[j].id_pano == res->panos[i].id_pano){
        res->panos[i].ids[k+1] = j;
        k++;
        if(k==res->panos[i].size)
          break;
      }

    /* binary search */
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

int new_pbdata(sqlite3* db, pbdata_s* res){
  int nb_items, nb_bonuses, nb_panos;
  int ret;
  nb_items = select_count_items(db);
  nb_panos = select_count_panos(db);
  nb_bonuses = select_count_bonuses(db);
  res->nb_items = nb_items;
  res->nb_panos = nb_panos;
  res->nb_bonuses = nb_bonuses;

  /*WARNING*/
  res->items_data = (statline_s*) malloc((nb_items+1)*sizeof(statline_s));

  ret = get_items_stats(db, res->items_data);

  res->bonuses_data = (statline_s*) malloc(nb_bonuses*sizeof(statline_s));

  ret = get_bonuses(db, res->bonuses_data);

  res->panos = (pids_s*) malloc(nb_panos*sizeof(pids_s));

  ret = get_panos_info(db, res);

  fill_panos_id(res);

  return ret;
}

void free_pbdata(pbdata_s* pbd){
  int i;
  for(i=0; i<pbd->nb_panos; i++)
    free(pbd->panos[i].ids);

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
  printf("ITEMS [%d]\n", pbd->nb_items);
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
  int j;
  for(j=0; j<n; j++){
    printf("%f\t", vec[j]);
  }
  printf("\n");
}

void add_vec(size_t n, double v1[], double v2[], double output[]){
  int i;
  for(i=0; i<n; i++)
    output[i] = v1[i]+v2[i];
}

void scalar_mult_vec(size_t n, double lambda, double input[], double output[]){
  int i;
  for(i=0; i<n; i++)
    output[i] = lambda*input[i];
}

void print_matrix(size_t n, size_t m, double matrix[]){
  int i, j;
  for(i=0; i<n; i++){
    for(j=0; j<m; j++){
      printf("%f\t", matrix[i*m+j]);
    }
    printf("\n");
  }
}

int stat_to_basis(size_t n, size_t m, double matrix[], 
    double input[], double output[]){
  int i, j;

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

int basis_to_stat(size_t n, size_t m, double matrix[],
    double input[], double output[]){
  int i, j;

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

linprob_s* new_linprob(pbdata_s* pbd){
  int i, j, count;

  linprob_s* res = malloc(sizeof(linprob_s));
  res->n = pbd->nb_items + pbd->nb_bonuses;
  res->m = STATS_COUNT;
  res->pb = glp_create_prob();
  glp_create_index(res->pb);
  res->matrix = malloc(sizeof(double) * res->n * res->m);
  for(i=0; i<pbd->nb_items; i++)
    for(j=0; j<res->m; j++)
      res->matrix[i*res->m + j] = pbd->items_data[i+1].stats[j];

  for(i=0; i<pbd->nb_bonuses; i++)
    for(j=0; j<res->m; j++)
      res->matrix[(i+pbd->nb_items)*res->m + j] = pbd->bonuses_data[i].stats[j];

  glp_set_prob_name(res->pb, "miniprob");
  glp_set_obj_name(res->pb, "maxfo");
  glp_set_obj_dir(res->pb, GLP_MAX);
  glp_add_cols(res->pb, res->n);

  /*adding items variables*/
  for(i=1; i<pbd->nb_items+1; i++){
    glp_set_col_name(res->pb, i, pbd->items_data[i].name);
    glp_set_col_kind(res->pb, i, GLP_BV);
    /*off set rings*/
    if(pbd->items_data[i].slot_code == slots_codes[SLOT_RING] &&
        !pbd->items_data[i].id_pano){
      glp_set_col_kind(res->pb, i, GLP_IV);
      glp_set_col_bnds(res->pb, i, GLP_DB, 0., 2.);
    }
  }


  /*adding bonuses variables*/
  for(i=0; i < pbd->nb_bonuses; i++){
    glp_set_col_name(res->pb, i+1+pbd->nb_items, pbd->bonuses_data[i].name);
    glp_set_col_kind(res->pb, i+1+pbd->nb_items, GLP_BV);
  }
  /*slot constraints*/
  /* TODO IMPROVE*/
  char* stnames[11] = {"amulet", "hat", "ring", "weapon", "shield", 
    "belt", "back", "boots", "dofus", "prysma", "pet"};
  double vec[pbd->nb_items+1];
  for(i=0; i<pbd->nb_items; i++) vec[i+1] = 1.;

  glp_add_rows(res->pb, 11);
  int ids[pbd->nb_items+1];
  for(i=0; i<11; i++){
    count = 1;
    for(j=1; j<pbd->nb_items+1; j++){
      if(pbd->items_data[j].slot_code == i){
        ids[count] = j;
        count ++;
      }
    }

    glp_set_row_name(res->pb, i+1, stnames[i]);
    glp_set_row_bnds(res->pb, i+1, GLP_UP, 0, 1);
    glp_set_mat_row(res->pb, i+1, count-1, ids, vec);
  }

  glp_set_row_bnds(res->pb, slots_codes[SLOT_RING] +1, GLP_UP, 0, 2);
  glp_set_row_bnds(res->pb, slots_codes[SLOT_DOFUS] +1, GLP_UP, 0, 6);
  glp_set_row_bnds(res->pb, slots_codes[SLOT_PRISMA] +1, GLP_UP, 0, 1);

  glp_add_rows(res->pb, 1);
  count = 1;
  for(j=1; j<pbd->nb_items+1; j++){
    int code = pbd->items_data[j].slot_code;
    if(code == slots_codes[SLOT_DOFUS] || code == slots_codes[SLOT_PRISMA]){
      ids[count] = j;
      count ++;
    }
  }
  i = glp_get_num_rows(res->pb);
  glp_set_row_name(res->pb, i, "DOFUS + PRISMA");
  glp_set_row_bnds(res->pb, i, GLP_UP, 0, 6);
  glp_set_mat_row(res->pb, i, count-1, ids, vec);

  /*sets (panos) constraints*/
  for(i=0; i<pbd->nb_panos; i++){
    add_pano_constraints(res->pb, pbd->panos[i]);
  }

  return res;
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

int set_obj_coeff(linprob_s* lp, double coeffs[]){
  double output[lp->n];
  stat_to_basis(lp->n, lp->m, lp->matrix, coeffs, output);

  int i;
  for(i=0; i<lp->n; i++) glp_set_obj_coef(lp->pb, i+1, output[i]);

  return 0;
}

int const_linear(linprob_s* lp, double coeffs[], const char* name){
  int i, ret;
  double output[lp->n+1];
  int ids[lp->n+1];
  for(i = 1; i<lp->n+1; i++) ids[i] = (double) i;
  
  stat_to_basis(lp->n, lp->m, lp->matrix, coeffs, output+1);
  ret = glp_add_rows(lp->pb, 1);
  glp_set_mat_row(lp->pb, ret, lp->n, ids, output);

  if(name)
    glp_set_row_name(lp->pb, ret, name);

  return ret;
}

int const_multi_simple_constraints(linprob_s* lp, double bounds[], int sign[]){
  double coeffs[STATS_COUNT] = {0};
  int i;
  for(i=0; i<lp->m; i++){
    if(bounds[i]>0.1 || -bounds[i]>0.1){
      coeffs[i] = 1.;
      if(sign[i] < 0)
        const_linear_lower(lp, coeffs, bounds[i], stats_names[i]);
      else if (sign[i]>0)
        const_linear_upper(lp, coeffs, bounds[i], stats_names[i]);
      else
        const_linear_fix(lp, coeffs, bounds[i], stats_names[i]);
      coeffs[i] = 0.;
    }
  }

  return 0; 
}

int const_linear_upper(linprob_s* lp, double coeffs[],
    double bound, const char* name){
  int ret = const_linear(lp, coeffs, name);
  glp_set_row_bnds(lp->pb, ret, GLP_UP, bound, bound);
  return ret;
}

int const_linear_lower(linprob_s* lp, double coeffs[],
    double bound, const char* name){
  int ret = const_linear(lp, coeffs, name);
  glp_set_row_bnds(lp->pb, ret, GLP_LO, bound, bound);
  return ret;
}

int const_linear_fix(linprob_s* lp, double coeffs[],
    double bound, const char* name){
  int ret = const_linear(lp, coeffs, name);
  glp_set_row_bnds(lp->pb, ret, GLP_FX, bound, bound);
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

void print_linsol(linprob_s* lp){
  int i;
  for(i=1; i<lp->n + 1; i++){
    double val = glp_mip_col_val(lp->pb, i);
    if(val>0.5)
      printf("%s %f\n", glp_get_col_name(lp->pb, i), val);
  }
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
