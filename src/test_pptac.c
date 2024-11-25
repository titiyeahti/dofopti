#include "glpapi.h"
#define DBFILE "skrapipi/pydb.db"

int main(void){
  int ret, i;
  sqlite3* db;
  pbdata_s pbd;
  linprob_s* lp;
  stat_vect bs = {0};

  init();
  ret = sqlite3_open(DBFILE, &db);
  new_pbdata(db, &pbd, NULL, NULL, 90);
  ret = sqlite3_close(db);
  lp = new_linprob(&pbd);
  free_pbdata(&pbd);
  double coeffs[STATS_COUNT] = {0};
  coeffs[INT] = 1.;

  set_obj_coeff(lp, coeffs);


  int sign[STATS_COUNT] = {0};
  coeffs[INT] = 0.;
  sign[PA] = 0; coeffs[PA] = 8.;
  sign[PM] = 0; coeffs[PM] = 4.;
  sign[RE_PER_AIR] = -1; coeffs[RE_PER_AIR] = 20.;
  sign[RE_PER_FEU] = -1; coeffs[RE_PER_FEU] = 20.;
  sign[RE_PER_TERRE] = -1; coeffs[RE_PER_TERRE] = 20.;
  sign[RE_PER_EAU] = -1; coeffs[RE_PER_EAU] = 20.;
  sign[RE_PER_NEUTRE] = -1; coeffs[RE_PER_NEUTRE] = 20.;
  const_multi_simple_constraints(lp, coeffs, sign);
  solve_linprob(lp);
  glp_print_mip(lp->pb, "pptac.txt");
  print_linsol(lp);

  free_linprob(lp);
  return EXIT_SUCCESS;
}
