#include "glpapi.h"
#define DBFILE "skrapipi/pydb.db"

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
  coeffs[VITA] = 100.;

  set_obj_coeff(lp, coeffs);


  int sign[STATS_COUNT] = {0};
  coeffs[VITA] = 0.;
  sign[PA] = 0; coeffs[PA] = 3.;
  sign[PM] = 0; coeffs[PM] = 2.;
  sign[RE_PER_AIR] = -1; coeffs[RE_PER_AIR] = 40.;
  sign[RE_PER_FEU] = -1; coeffs[RE_PER_FEU] = 40.;
  sign[RE_PER_TERRE] = -1; coeffs[RE_PER_TERRE] = 40.;
  sign[RE_PER_EAU] = -1; coeffs[RE_PER_EAU] = 40.;
  sign[RE_PER_NEUTRE] = -1; coeffs[RE_PER_NEUTRE] = 40.;
  const_multi_simple_constraints(lp, coeffs, sign);
  const_lock_in_item(lp, "rykke errel's bravery");
  const_lock_in_item(lp, "ivory dofus");
  const_lock_in_item(lp, "dokoko");
  const_lock_in_item(lp, "abyssal dofus");
  const_lock_in_item(lp, "lavasmith dofus");
  const_lock_in_item(lp, "sparkling silver dofus");
  const_lock_in_item(lp, "emerald dofus");
  const_lock_out_item(lp, "claymomore");
  solve_linprob(lp);
  glp_print_mip(lp->pb, "mipSacri.txt");
  print_linsol(lp);

  free_linprob(lp);
  return EXIT_SUCCESS;
}
