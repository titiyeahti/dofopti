#ifndef DATA_TYPES_H
#define DATA_TYPES_H
#define NAME_LEN 48
/* 4*4+2 */
#define NB_COLS_FOR_STATS 18
#define NB_COLS_PER_ELEM 4
#define NB_ELEMS 4

#define ERR_MSG(s) fprintf(stderr, "%s:%d-%s\n", __FILE__, __LINE__, s)
#define SQL_CHECK_ERRORS(ret) if(ret == SQLITE_ERROR) {ERR_MSG("SQL"); exit(1);} 


#define DEF(e, str, s) e

enum stats_e{
#include "stats.conf"
};

#undef DEF

#define DEF(e, c, s, cs) e

enum slots_e{
#include "slots.conf"
};

#undef DEF

struct constree{
  enum Type {LEAF, BRACES, OR, AND} t;
  union {
    struct {
      int stat;
      int sign;
      int val;
    } leaf;

    struct constree* braces;

    struct {
      struct constree* lm;
      struct constree* rm;
    } node;
  };
};

typedef struct constree constree_s;

/* We will cast into double when entering glpk*/
typedef int stat_vect[STATS_COUNT];
typedef char short_word[NAME_LEN];

/* Contains stats for a given item or specific set bonus */
typedef struct {
  stat_vect stats;
  int id, id_pano, slot_code;
  short_word name;
} statline_s;

/* Used to keep a link between the sets (panos) and the items part of them*/
typedef struct {
  short_word name;
  int id_pano, size, maxbonuses;
  /* len(ids) = size + maxbonuses */
  int ids[20];
} pids_s;

typedef struct {
  int level, nb_items, nb_bonuses, nb_panos;
  int targeted_slots[SLOT_COUNT];
  constree_s** items_consts;
  stat_vect base_stats;
  statline_s* items_data;
  statline_s* bonuses_data;
  pids_s* panos; 
} pbdata_s;

typedef struct {
  int m, n;
  double* matrix;
  glp_prob* pb;
} linprob_s;

#endif
