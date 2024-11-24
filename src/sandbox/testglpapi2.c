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
  coeffs[PA] = 1.;
  const_linear_fix(lp, coeffs, 4., "12 pa"); 
  coeffs[PA] = 0.;
  coeffs[PM] = 1.;
  const_linear_fix(lp, coeffs, 2., "6 pm"); 
  coeffs[PM] = 0.;
  coeffs[VITA] = 1.;
  const_linear_lower(lp, coeffs, 2555., "min vita"); 
  coeffs[VITA] = 0.;
  coeffs[RE_PER_AIR] = 1.;
  const_linear_lower(lp, coeffs, 25., "min re air"); 
  coeffs[RE_PER_TERRE] = 1.;
  coeffs[RE_PER_AIR] = 0.;
  const_linear_lower(lp, coeffs, 25., "min re terre"); 
  coeffs[RE_PER_FEU] = 1.;
  coeffs[RE_PER_TERRE] = 0.;
  const_linear_lower(lp, coeffs, 25., "min re feu"); 
  coeffs[RE_PER_FEU] = 0.;
  coeffs[RE_PER_EAU] = 1.;
  const_linear_lower(lp, coeffs, 25., "min re eau"); 
  coeffs[RE_PER_NEUTRE] = 1.;
  coeffs[RE_PER_EAU] = 0.;
  const_linear_lower(lp, coeffs, 20., "min re neutre"); 
  solve_linprob(lp);
  glp_print_mip(lp->pb, "mip2.txt");
  print_linsol(lp);

  free_linprob(lp);
  return EXIT_SUCCESS;
}
