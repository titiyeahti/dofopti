#ifndef PROBLEM_H
#define PROBLEM_H

#include "data_import.h"
#include <glpk.h>

/* need of intemrediate structure ? */
/* not mandatory */

/* problem is entrirely defined by 
 * the subset of items,
 * the objective function -> array of stats, 
 * user constraints
    * stats consts    
    * items consts
    * linear form consts
 * the base stats
 */

/* data of the problem 
 * N the number of items
 * P the number of panos
 * B the number of bonuses
 * SI the number of set items
 * char item_names[N][32] items names
 * char panos_names[P][32] panos names
 * size_t slot_table[SLOT_COUNT+1] :
    * slot_table[i] contains the id of the first item for the slot i
    * slot_table[SLOT_COUNT] contains N-1
 * size_t panos_items_delim[P+1]
    * panos_items_delim[i] the first items of the pano i in panos_items
    * panos_items_delim[P] SI - 1
 * size_t panos_items[SI]
    * item id order by pano
 * size_t 
 * double S[1+18+N+B][STATS_COUNT] s.t.
 *  TODO take into account stat contribtion for
 *  tacle fuite ret pa pm res pa pm pods pp soin
 *  (whenever a contributing  stat is met, add the value * ratio to the
 *  corresponding stat) -> create a translator function
    * S[i][j] = value of stat i given by problem variable j.
        * 1 col for base stat value
        * 18 columns reserved for the carac repartition
        * N cols for items stats.
        * B pano bonuses.
 * objective function
 * TODO item constraints (select count -> table)
 * TODO manage set bonuses limit -> one inequality ? 
        * (0-1) sum of concerned trophies / count <= 2 - sum 2-bonus pano- 2sum 3+ bonus pano
        * add directly to glpk
 * TODO user constraints
        * convert (matrix mul) and add directly to glpk
 * number of cols -> 
 *  18 for carac distribution -> integer [0,100]
 *    4*4 (for, cha, agi, int) 
 *    2 (sasa, vita)
 *  N for items -> binary, equiped or not
 *  B for bonuses -> binary, active or not 
 * number of rows
 *  11 for slots 
 *  2*P for panos (2 equalities)
 *  sum var_carac * cout  <= 5*level
 *  total 2*P + 12
 *  + trophies set_bonus constraints
 *  + item constraints
 *  + user constraints
 */

/* be able to convert a linear combination of stats to a linear combination of
    * pano bonuses
    * items
    * base stats
 * multiply the combination by the matrix (y*S) */

#define ITEM_STATS_OFFSET 19

/* TODO create a temp table for concerned panos */
/* TODO check sql requests */

#define SQL_CREATE_WORK_TABLE(name, filter) \
  "create table if not exists work_" name " as select * from items as "\
  "base_items where (" filter ") order by slot_code;"

#define SQL_ADD_TEMPID_COL(name) \
  "alter table work_" name " add tempid integer;"

#define SQL_FILL_TEMPID(name) \
  "update table work_" name " set tempid = rowid - 1;"

#define SQL_DROP_WORK_TABLE(name) \
  "drop table if exists work_" name ";"

#define SQL_WORK_COUNT_ITEMS(name) \
  "select count(*) from work_" name ";"

#define SQL_WORK_COUNT_PANOS(name) \
  "select count(distinct id_pano) from work_" name ";"

#define SQL_WORK_COUNT_SETITEMS(name) \
  "select count(*) from work_" name " where id_pano is not null;"

#define SQL_WORK_COUNT_BONUSES(name) \
  "select sum(nb_bon) from (select max(nb_items) + 1 as nb_bon, "\
  "bonuses.id_pano from work_" name " join bonuses on work_" name\
  ".id_pano = bonuses.id_pano group by bonuses.id_pano);

#define SQL_WORK_SELECT_ITEM_DELIM(name) \
  "select slot_code, count(*) from work_" name " group by slot_code;"

#define SQL_WORK_SELECT_ITEM_STATS(name) \
  "select tempid, stat_code, maxval from work_" name " join stats on"\
  "id = id_items order by tempid, stat_code;"

#define SQL_WORK_SELECT_PANO_ITEMS(name) \
  "select tempid from work_" name " where id_pano is not null order "\
  "by id_pano;"

/* TODO fix */
#define SQL_WORK_SELECT_PANO_BONUSES(name) \


#define SQL_WORK_SELECT_PANO_BONUSES(name) \
  "select max(nb_items) + 1 as nb_bon, "\
  "bonuses.id_pano from work_" name " join bonuses on work_" name\
  ".id_pano = bonuses.id_pano group by bonuses.id_pano order by bonuses.id_pano;"

typedef char short_word[32];
typedef double stat_vector[STATS_COUNT];

typedef struct problem{
  sqlite3* db;
  short_word pb_name;
  char* sql_query;
  int lvl;
  stat_vector obj_fun;
  int opt_dir;

  size_t nb_items;
  size_t nb_panos;
  size_t nb_set_items;
  size_t nb_bonuses;

  size_t* items_id;
  
  /* to ease human reading of the problem */
  short_word* item_names;
  short_word* panos_names;

  size_t slot_limit[SLOT_COUNT+1];
  size_t* panos_delim; /*size nb_panos+1*/
  size_t* panos_item; /*size nb_set_items*/
  size_t* panos_bonuses_delim; /*size nb_panos+1*/
  stat_vector* coeff_matrix; /*size = 1 + 18 + nb_items + nb_bonuses */

  /* maybe going to disappear*/
  size_t last_col;
  size_t last_row;
  glp_prob* mip_prob;
} problem_t;

problem_t* problem_new(sqlite3* db, const char* sql_query, 
    const char* name, int lvl);

/* TODO query wrapper */

/* TODO row creation wrapper */

int problem_create_temp_table(problem_t* pb);//

int problem_select_count(problem_t* pb);//

int problem_slot_delim(problem_t* pb);//

int problem_item_stats(problem_t* pb);//

int problem_item_names(problem_t* pb);

int problem_pano_names(problem_t* pb);

int problem_pano_items(problem_t* pb);//

int problem_pano_bonuses(problem_t* pb);//

int problem_pano_bonuses_delim(problem_t* pb);

int problem_add_cols(problem_t* pb);

int problem_add_structural_rows(problem_t* pb);

int problem_add_pano_rows(problem_t* pb);

int problem_set_obj(problem_t* pb, stat_vector obj_coeff, int opt_dir);

int problem_add_user_rows(problem_t* pb, stat_vector const_coeff,
    int type, double lb, double ub);

int problem_add_item_rows(problem_t* pb, int stat_code, int type, 
    double lb, double ub);

int problem_solve(problem_t* pb);

/* free every byte allocated by problem_new and drop the table created at the
 * start*/
void free_problem(problem_t* pb);

#endif
