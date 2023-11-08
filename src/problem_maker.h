#ifndef PROBLEM_MAKER_H
#define PROBLEM_MAKER_H

#include "data_import.h"
#include <list>
#include <glpk.h>

typedef struct constraint{
  /* constraint example : MP >= 5
  -> {31, 1, 5.0} */
  int stat_code;
  /* <= : -1; = : 0; >= : 1*/
  int const_type;
  double value;
} const_t;

typedef struct item{
  size_t item_id;
  int slot;

  /* index of the binary variable tied to this item in the linear problem*/
  size_t colum_index;
  size_t nb_consts;
  size_t pano_id;
  pano_t* pano;
  /* not usefull */
  //double stats[STATS_COUNT];
  const_t item_constraints[nb_consts]; 
} item_t;

typedef struct pano{
  size_t pano_id;
  size_t size;
  size_t nb_items;
  size_t item_ids[nb_items];
  item_t* items[nb_items];

  /* indexes of column variables for each bonus in the linear problem*/
  size_t bonuses_index[size+1];
  /* Not usefull */
  //double bonuses[size][STATS_COUNT];
} pano_t;

/* TODO using list is a bad idea in the fianl structure even if it could 
 * help as temporary struct during computations */
/* TODO add two column in pano table : nb_item and size (may be different)
 */

typedef struct problem{
  sqlite3* db;

  /* objective function */
  double obj[STATS_COUNT];

  /* statistics without equipmenent and carac points */
  double base_stats[STATS_COUNT];

  /* direction of optimization */
  int opt_dir;

  /* list of user constraints */
  std::list<const_t> user_consts;

  /* index of the first item of the slot */
  size_t item_delim[SLOTS_COUNT+1];

  /* array of item indices */
  item_t item_array[nb_items];

  /* array of panos */
  pano_t pano_array[nb_panos];

  /* TODO allow to efficiently create panos constraints in glpk 
   *  keep track of column id for items in each pano*/

  /* TODO create arrays to keep track of item/bonuses contribution to stats*/
  /* use c++ std array to get find ? */
  /* or create an array, sort it and proceed to binary search */

  /* GLPK problem */
  glp_prob* prob;

} problem_t;

item_t* item_new();

int item_from_id(sqlite3* db, item_t* item, size_t id);

void item_free();

pano_t* pano_new();

int pano_from_id(sqlite3* db, pano_t* pano, size_t id);

void pano_free();

problem_t* problem_new();

int problem_set_obj_coeff(problem_t* p, size_t stat_code, double coeff);

int problem_set_opt_dir(problem_t* p, int dir);

int problem_fetch_data(problem_t* p, const char* sql_condition);

int problem_to_glpk(problem_t* p);

int problem

#endif
