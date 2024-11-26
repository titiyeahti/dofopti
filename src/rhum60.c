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
  tgtslt[SLOT_DOFUS] = 0;
  tgtslt[SLOT_RING] = 2;
  tgtslt[SLOT_PET] = 1;
  stat_vect bs = {0};
  bs[CHA] = 100;
  bs[AGI] = 100+60;
  bs[PA] = 6;
  bs[PM] = 3;


  init();
  ret = sqlite3_open(DBFILE, &db);
  new_pbdata(db, &pbd, NULL, tgtslt, 65);
  ret = sqlite3_close(db);
  lp = new_linprob(&pbd);
  double coeffs[STATS_COUNT] = {0};

  coeffs[AGI] = .32;
  coeffs[PUI] = .555;
  coeffs[DO_POU] = 2.5;
  coeffs[DO] = 2.;
  coeffs[DO_AIR] = 2.;
  coeffs[DO_EAU] = .1;
  coeffs[CHA] = .235;
  set_obj_coeff(lp, coeffs);
  coeffs[PUI] = .0;
  coeffs[AGI] = 0.;
  coeffs[DO_POU] = 0.;
  coeffs[DO] = 0.;
  coeffs[DO_AIR] = 0.;
  coeffs[DO_EAU] = 0.;
  coeffs[CHA] = .0;

  int sign[STATS_COUNT] = {0};
  sign[PA] = 0; coeffs[PA] = 9.;
  /*sign[PM] = 0; coeffs[PM] = 4.;*/

  const_multi_simple_constraints(lp, coeffs, sign);
  solve_linprob(lp);
  glp_print_mip(lp->pb, "rhum.txt");
  print_linsol(lp, &pbd);
  free_pbdata(&pbd);

  free_linprob(lp);
  return EXIT_SUCCESS;
}
