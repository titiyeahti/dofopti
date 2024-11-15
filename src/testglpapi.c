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
  print_pbdata(&pbd);
  free_pbdata(&pbd);
  ret = sqlite3_close(db);
  double coeffs[STATS_COUNT] = {0};
  coeffs[DO_NEUTRE] = 1.;
  coeffs[PUI] = .705;
  coeffs[FOR] = .705;
  coeffs[DO_CRIT] = 0.5;

  set_obj_coeff(lp, coeffs);
  const_lock_in_item(lp, "Musamune");
  coeffs[DO_NEUTRE] = 0;
  coeffs[PUI] = 0;
  coeffs[FOR] = 0;
  coeffs[DO_CRIT] = 0;
  coeffs[CRIT] = 1.;
  const_linear_lower(lp, coeffs, 40., "min crit"); 
  coeffs[CRIT] = 0.;
  coeffs[PA] = 1.;
  const_linear_fix(lp, coeffs, 4., "min pa"); 
  coeffs[PA] = 0.;
  coeffs[PM] = 1.;
  const_linear_fix(lp, coeffs, 2., "min pm"); 
  solve_linprob(lp);
  glp_print_mip(lp->pb, "mip1.txt");
  print_linsol(lp);

  free_linprob(lp);
  return EXIT_SUCCESS;
}
