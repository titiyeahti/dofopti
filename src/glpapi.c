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
  ret = sqlite3_prepare_v2(db, "SELECT count(distinct id) FROM items join stats on items.id = id_item;", -1, &stmt, NULL);
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
      "select id_item, stat_code, maxval, name, id_pano from items join stats on id = id_item order by id_item;",
      -1, &stmt, NULL);

  int last_id = -1;
  int id = -1;
  int count = -1;
  while(sqlite3_step(stmt) == SQLITE_ROW){
    id = sqlite3_column_int(stmt, 0);
    if (id != last_id){
      count++;
      items_data[count].id_pano = sqlite3_column_int(stmt, 4);
      strcpy(items_data[count].name, (const char*) 
          sqlite3_column_text(stmt, 3));
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
      "select count(distinct id), items.id_pano, max(nb_items) from items join bonuses on bonuses.id_pano = items.id_pano group by items.id_pano;",
      -1, &stmt, NULL);

  int count = 0;
  while(sqlite3_step(stmt) == SQLITE_ROW){
    res->panos[count].id_pano = sqlite3_column_int(stmt, 1);
    res->panos[count].size = sqlite3_column_int(stmt, 0);
    res->panos[count].maxbonuses = sqlite3_column_int(stmt, 2);

    res->panos[count].ids = 
      (int*) malloc(sizeof(int)*(res->panos[count].size + 
            res->panos[count].maxbonuses));
    count ++;
  }

  ret = sqlite3_finalize(stmt);
  return ret;
}

int fill_panos_id(pbdata_s* res){
  int i, j, k;
  for(i = 0; i < res->nb_panos; i++){
    /*items*/
    /*printf("PANO id : %d\n", res->panos[i].id_pano);*/
    k = 0;
    for(j = 0; j < res->nb_items; j++)
      if(res->items_data[j].id_pano == res->panos[i].id_pano){
        res->panos[i].ids[k] = j;
        k++;
        if(k==res->panos[i].size)
          break;
      }

    /* binary search */
    k = 0;
    for(j = 0; j < res->nb_bonuses; j++){
      if(res->bonuses_data[j].id_pano == res->panos[i].id_pano){
        res->panos[i].ids[k+res->panos[i].size] = j;
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

  res->items_data = (statline_s*) malloc(nb_items*sizeof(statline_s));

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
    printf("%d ", pi->ids[i]);
  printf(")(");
  for(i = pi->size; i < pi->size + pi->maxbonuses; i++)
    printf("%d ", pi->ids[i]);

  printf(")\n");
}

void print_pbdata(pbdata_s* pbd){
  int i;
  printf("ITEMS [%d]\n", pbd->nb_items);
  for(i = 0; i < pbd->nb_items; i++){
    printf("%d\t", i);
    print_statline(&(pbd->items_data[i]));
  }

  printf("\nBONUSES [%d]\n", pbd->nb_bonuses);
  for(i = 0; i < pbd->nb_bonuses; i++){
    printf("%d\t", i);
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


lineprob_s* new_linprob(sqlite3* db){
  lineprob_s* res = malloc(sizeof(lineprob_s));
  new_pbdata(dn, &(res->pbd));
  /* number of columns (variables) */
  res->n = res->pbd.nb_items + res->pbd.nb_bonuses;
  res->m = STATS_COUNT;
  res->pb = glp_create_prob();
  res->matrix = malloc(sizeof(double) * res->n * res->m);

  /* TODO subject to changes */
  glp_set_prob_name(res->pb, "miniprob");
  glp_set_obj_name(res->pb, "maxfo");
  glp_set_obj_dir(res->pb, GLP_MAX);
  glp_add_cols(res->pb, res->n);

  /* naming cols */
  int i;
  for(i=0; i<res->pbd.nb_items; i++)
    glp_set_col_name(res->pb, i+1, res->pbd.items_data[i].name);
  for(i=res->pbd.nb_items; i < res->n; i++)
    glp_set_col_name(res->pb, i+1, res->pbd.items_data[i].name);
  


  return res;
}

void free_linprob(lineprob_s* lp){

  free(lp);
}
