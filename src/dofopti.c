#include "glpapi.h"
#include "reader.h"

#define DBFILE "skrapipi/pydb.db"

int main(int argc, char* argv[]){
  if(argc != 2) exit(1);

  sqlite3* db;
  pbdata_s pbd;
  linprob_s* lp;

  int i, ret, lvl;
  int tgt_slots[SLOT_COUNT] = {1};
  int sign[STATS_COUNT];
  double obj_coeff[STATS_COUNT];
  double bnds[STATS_COUNT];
  stat_vect bs = {0};
  for(i=0; i<SLOT_COUNT; i++) tgt_slots[i] = 1;
  tgt_slots[SLOT_DOFUS] = 6;
  tgt_slots[SLOT_RING] = 2;

  for(i=0; i<STATS_COUNT; i++){
    sign[i] = 0;
    obj_coeff[i] = 0.;
    bnds[i] = 0.;
    bs[i] = 0.;
  }

  init();
  ret = sqlite3_open(DBFILE, &db);
  ret = reader(argv[1], &lvl, bs, tgt_slots, obj_coeff, bnds, sign);


  new_pbdata(db, &pbd, bs, tgt_slots, lvl);
  ret = sqlite3_close(db);
  lp = new_linprob(&pbd);

  set_obj_coeff(lp, obj_coeff);
  const_multi_simple_constraints(lp, bnds, sign);

  solve_linprob(lp);
  print_linsol(lp, &pbd);

  free_pbdata(&pbd);
  free_linprob(lp);

  return 0;
}
