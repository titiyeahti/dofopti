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

int problem_create_temp_table(problem_t* pb);

int problem_select_count(problem_t* pb);

int problem_slot_delim(problem_t* pb);

int problem_item_stats(problem_t* pb);

int problem_item_names(problem_t* pb);

int problem_pano_names(problem_t* pb);

int problem_pano_items(problem_t* pb);

int problem_pano_bonuses(problem_t* pb);

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
