#include "problem.h"

problem_t* problem_new(sqlite3* db, const char* sql_query, 
    const char* name, int lvl){
  int ret;
  problem_t* pb;

  pb = (problem_t *) malloc(sizeof(problem_t));
  pb->db = db;
  strcpy(pb->pb_name, name);
  pb->lvl = lvl;
  pb->sql_query = (char*) malloc(strlen(sql_query) + 1);
  strcpy(pb->sql_query, sql_query);

  /* database manipulations */

  ret = problem_create_temp_tables(pb);
  ret = problem_select_count(pb);

  /* malloc arrays */
  
  pb->items_id = (size_t*) malloc(sizeof(size_t)*pb->nb_items);
  pb->item_names = (short_word*) malloc(sizeof(short_word)*pb->nb_items);
  pb->panos_names = (short_word*) malloc(sizeof(short_word)*pb->nb_panos);
  pb->panos_item = (size_t*) malloc(sizeof(size_t)*pb->nb_set_items);
  pb->panos_delim = (size_t*) malloc(sizeof(size_t)*(pb->nb_panos+1));
  pb->panos_delim_bonuses = (size_t*) malloc(sizeof(size_t)*(pb->nb_panos+1));

  pb->coeff_matrix = (stat_vector*) malloc(sizeof(stat_vector)*(
        ITEM_STATS_OFFSET + pb->nb_items + pb->nb_bonuses));

  /* creating glpk problem */

  pb->glp_prob = glp_create_prob();
  return pb;
}

/* free every byte allocated by problem_new and drop the table created at the
 * start*/
void free_problem(problem_t* pb){
  problem_drop_temp_tables(pb);
  free(pb->items_id);
  free(pb->item_names);
  free(pb->panos_names);
  free(pb->panos_item);
  free(pb->panos_delim);
  free(pb->panos_delim_bonuses);
  free(pb->coeff_matrix);
  
  free(pb->sql_query);

  glpk_delete_prob(pb->glp_prob);

  free(pb);
}

/* Once debug done, maybe concat each of these atomic sql instructions */
int problem_create_temp_tables(problem_t* pb){
  int ret;
  sqlite3_stmt* stmt;

  /* creating a subset */
  ret = sqlite3_prepare(pb->db, SQL_CREATE_WORK_TABLE(pb->pb_name, pb->sql_query), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if(ret) return ret; 

  ret = sqlite3_prepare(pb->db, SQL_ADD_TEMPID_COL(pb->pb_name), -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if(ret) return ret;

  ret = sqlite3_prepare(pb->db, SQL_FILL_TEMPID(pb->pb_name), -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if(ret) return ret;

  /* tempano table */
  ret = sqlite3_prepare(pb->db, SQL_CREATE_WORK_PANOS(pb->pb_name), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if(ret) return ret; 

  ret = sqlite3_prepare(pb->db, SQL_ADD_PTEMPID_COL(pb->pb_name), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if(ret) return ret;

  ret = sqlite3_prepare(pb->db, SQL_FILL_PTEMPID(pb->pb_name), -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if(ret) return ret;

  ret = sqlite3_prepare(pb->db, SQL_ADD_BCOUNT_COL(pb->pb_name), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if(ret) return ret;

  ret = sqlite3_prepare(pb->db, SQL_FILL_BCOUNT(pb->pb_name), -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  return ret;
}

int problem_drop_temp_tables(problem_t* pb){
  int ret;
  sqlite3_stmt* stmt;

  ret = sqlite3_prepare(pb->db, SQL_DROP_WORK_TABLE(pb->pb_name), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if(ret) return ret;

  ret = sqlite3_prepare(pb->db, SQL_DROP_WORK_PANOS(pb->pb_name), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  return ret;
}

/* TODO create a wrapper for count request */
int problem_select_count(problem_t* pb){
  int ret;
  sqlite3* stmt;
  ret = sqlite3_prepare(pb->db, SQL_WORK_COUNT_ITEMS(pb->pb_name), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);
  pb->nb_items = (size_t) sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);

  ret = sqlite3_prepare(pb->db, SQL_WORK_COUNT_PANOS(pb->pb_name), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);
  pb->nb_panos = (size_t) sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);

  ret = sqlite3_prepare(pb->db, SQL_WORK_COUNT_SETITEMS(pb->pb_name), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);
  pb->nb_set_items = (size_t) sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);

  ret = sqlite3_prepare(pb->db, SQL_WORK_COUNT_BONUSES(pb->pb_name), 
      -1, &stmt, NULL);
  ret = sqlite3_step(stmt);
  pb->nb_bonuses = (size_t) sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);

  return ret;
} 

int problem_slot_delim(problem_t* pb){
  int ret;
  int id;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(pb->db, SQL_WORK_SELECT_ITEM_DELIM(pb->pb_name), 
      -1, &stmt, NULL);

  pb->slot_limit[0] = (size_t) 0;

  while(sqlite3_step(stmt) == SQLITE_ROW){
    id = sqlite3_column_int(stmt, 0) + 1;
    pb->slot_limit[id] = pb->slot_limit[id-1] 
      + (size_t) sqlite3_column_int(stmt, 1);
  }

  sqlite3_finalize(stmt);

  return ret;
}

int problem_item_stats(problem_t* pb){
  int ret;
  int id;
  int stat_code;

  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(pb->db, SQL_WORK_SELECT_ITEM_STATS(pb->pb_name), 
      -1, &stmt, NULL);

  while(sqlite3_step(stmt) == SQLITE_ROW){
    id = sqlite3_column_int(stmt, 0) + ITEM_STATS_OFFSET;
    stat_code = sqlite3_column_int(stmt, 1);

    pb->coeff_matrix[id][stat_code] = (double) sqlite3_column_int(stmt, 2);
  }

  sqlite3_finalize(stmt);

  return ret;
}

int problem_item_names(problem_t* pb);

int problem_pano_names(problem_t* pb);

int problem_pano_items(problem_t* pb){
  int ret;
  int id = 0;

  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(pb->db, SQL_WORK_SELECT_PANO_ITEMS(pb->pb_name), 
      -1, &stmt, NULL);

  while(sqlite3_step(stmt) == SQLITE_ROW){
    pb->panos_item[id] = sqlite3_column_int(stmt, 0);
    id++;
  }

  sqlite3_finalize(stmt);

  return ret;
}

int problem_pano_bonuses(problem_t* pb){
  int ret;
  /* minus one is here to simplify the while loop*/
  int id = ITEM_STATS_OFFSET + (int) pb->nb_items - 1;
  int nb_items, last_nb = 0;
  int pano_id, last_pano = 0;
  int stat_code;

  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(pb->db, SQL_WORK_SELECT_PANO_BONUSES(pb->pb_name), 
      -1, &stmt, NULL);

  if(ret) return ret;

  /* when meeting a new pano, skip nb_items id*/
  /* when meeting a new nb_items, id++*/
  while(sqlite3_step(stmt) == SQLITE_ROW){
    pano_id = sqlite3_column_int(stmt, 0);
    nb_items = sqlite3_column_int(stmt, 1);

    /* Most likely +1 is a lie in fact not*/
    if(pano_id != last_pano) id += nb_items + 1;
    /* most likely id ++*/
    else if(nb_items > last_nb) id += nb_items - last_nb;

    stat_code = sqlite3_column_int(stmt, 2);
    pb->coeff_matrix[id][stat_code] = (double) sqlite3_column_int(stmt, 3);

    last_nb = nb_items;
    last_pano = pano_id;
  }

  sqlite3_finalize(stmt);

  return ret;
}

/* dealt with simultaneaeeanously with the other delim by
 * requesting temp pano table*/
int problem_pano_bonuses_delim(problem_t* pb){
  int ret;
  return ret;
}

/* GLPK area */
int problem_add_cols(problem_t* pb);

int problem_add_structural_rows(problem_t* pb);

int problem_add_pano_rows(problem_t* pb);

/* Matrix mul to change basis */
int problem_set_obj(problem_t* pb, stat_vector obj_coeff, int opt_dir);


/* Not implemented for the beta version */
/* User constraints */
int problem_add_user_rows(problem_t* pb, stat_vector const_coeff,
    int type, double lb, double ub);

/* Item constraints */
int problem_add_item_rows(problem_t* pb, int stat_code, int type, 
    double lb, double ub);

int problem_solve(problem_t* pb);

