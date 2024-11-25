#include "glpapi.h"
#define DBFILE "skrapipi/pydb.db"

int main(void){
  int ret, i;
  sqlite3* db;
  pbdata_s pbd;
  linprob_s* lp;
  int tgtslt[SLOT_COUNT] = {1};
  for(i=0; i<SLOT_COUNT; i++){
    tgtslt[i]=1;
  }
  tgtslt[SLOT_DOFUS] = 2;
  stat_vect bs = {0};
  bs[VITA] = 2145;
  bs[PA] = 8;
  bs[PM] = 4;
  bs[AGI] = 100;


  init();
  ret = sqlite3_open(DBFILE, &db);
  new_pbdata(db, &pbd, bs, tgtslt, 200);
  ret = sqlite3_close(db);
  lp = new_linprob(&pbd);
  double coeffs[STATS_COUNT] = {0};

  coeffs[VITA] = 1.;
  set_obj_coeff(lp, coeffs);
  coeffs[VITA] = 0.;

  int sign[STATS_COUNT] = {0};
  sign[PA] = 0; coeffs[PA] = 11.;
  sign[PM] = 0; coeffs[PM] = 6.;
  sign[RE_PER_AIR] = -1; coeffs[RE_PER_AIR] = 48.;
  sign[RE_PER_FEU] = -1; coeffs[RE_PER_FEU] = 48.;
  sign[RE_PER_TERRE] = -1; coeffs[RE_PER_TERRE] = 48.;
  sign[RE_PER_EAU] = -1; coeffs[RE_PER_EAU] = 48.;
  sign[RE_PER_NEUTRE] = -1; coeffs[RE_PER_NEUTRE] = 45.;
  const_multi_simple_constraints(lp, coeffs, sign);
  const_lock_in_item(lp, "ivory dofus");
  const_lock_in_item(lp, "emerald dofus");
  const_lock_out_item(lp, "heavy burden");
  const_lock_out_item(lp, "signet of fate");
  const_lock_out_item(lp, "eternal chase");
  const_lock_out_item(lp, "howling souls");
  const_lock_out_item(lp, "cire's sorrow");
  solve_linprob(lp);
  glp_print_mip(lp->pb, "vita.txt");
  print_linsol(lp, &pbd);
  free_pbdata(&pbd);

  free_linprob(lp);
  return EXIT_SUCCESS;
}
