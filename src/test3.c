#include "glpapi.h"
#define DBFILE "bdd/testdb.db"

int main(void){
  int ret, i;
  sqlite3* db;
  pbdata_s pbd;
  linprob_s* lp;
  ret = sqlite3_open(DBFILE, &db);

  new_pbdata(db, &pbd);
  /*
  print_pbdata(&pbd);
  */
  lp = new_linprob(&pbd);
  free_pbdata(&pbd);
  ret = sqlite3_close(db);
  double coeffs[STATS_COUNT] = {0};
  coeffs[DO_EAU] = 4.;
  coeffs[PUI] = 1.05;
  coeffs[CHA] = 1.05;

  set_obj_coeff(lp, coeffs);
  coeffs[DO_EAU] = 0.;
  coeffs[CHA] = 0.;
  coeffs[PUI] = 0.;

  int sign[STATS_COUNT] = {0};
  sign[PA] = 0; coeffs[PA] = 4.;
  sign[PM] = 0; coeffs[PM] = 2.;
  sign[VITA] = -1; coeffs[VITA] = 2555.;
  sign[RE_PER_AIR] = -1; coeffs[RE_PER_AIR] = 25.;
  sign[RE_PER_TERRE] = -1; coeffs[RE_PER_TERRE] = 25.;
  sign[RE_PER_EAU] = -1; coeffs[RE_PER_EAU] = 25.;
  sign[RE_PER_FEU] = -1; coeffs[RE_PER_FEU] = 25.;
  sign[RE_PER_NEUTRE] = -1; coeffs[RE_PER_NEUTRE] = 20.;
  const_multi_simple_constraints(lp, coeffs, sign);
  solve_linprob(lp);
  glp_print_mip(lp->pb, "mip3.txt");
  print_linsol(lp);

  free_linprob(lp);
  return EXIT_SUCCESS;
}
