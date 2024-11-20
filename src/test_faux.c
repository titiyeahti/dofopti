#include "glpapi.h"
#define DBFILE "bdd/db2.db"

int main(void){
  int ret, i;
  sqlite3* db;
  pbdata_s pbd;
  linprob_s* lp;
  ret = sqlite3_open(DBFILE, &db);

  new_pbdata(db, &pbd);
  /*
   *   print_pbdata(&pbd);
   *     */
  lp = new_linprob(&pbd);
  free_pbdata(&pbd);
  ret = sqlite3_close(db);
  double coeffs[STATS_COUNT] = {0};
  coeffs[DO_EAU] = 1.;
  coeffs[DO] = 5.;
  coeffs[DO_TERRE] = 1.;
  coeffs[DO_AIR] = 1.;
  coeffs[DO_FEU] = 1.;
  coeffs[DO_CRIT] = 5.;
  coeffs[PUI] = 0.185*5;
  coeffs[CHA] = 0.185;
  coeffs[INT] = 0.185;
  coeffs[FOR] = 0.37;
  coeffs[AGI] = 0.185;

  set_obj_coeff(lp, coeffs);
  coeffs[DO_EAU] = 0.;
  coeffs[DO] = 0.;
  coeffs[DO_TERRE] = 0.;
  coeffs[DO_AIR] = 0.;
  coeffs[DO_FEU] = 0.;
  coeffs[DO_CRIT] = 0.;
  coeffs[PUI] = 0.;
  coeffs[CHA] = 0.;
  coeffs[INT] = 0.;
  coeffs[FOR] = 0.;
  coeffs[AGI] = 0.;
  coeffs[PA] = 1.;
  const_linear_fix(lp, coeffs, 2., "12 pa"); 
  coeffs[PA] = 0.;
  coeffs[CRIT] = 1.;
  const_linear_fix(lp, coeffs, 98., "100 critical"); 
  coeffs[CRIT] = 0.;
  const_lock_in_item(lp, "blord warrior's cursed scythe");

  solve_linprob(lp);
  glp_print_mip(lp->pb, "mip4.txt");
  print_linsol(lp);

  free_linprob(lp);
  return EXIT_SUCCESS;
}
