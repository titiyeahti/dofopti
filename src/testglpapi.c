#include "glpapi.h"
#define DBFILE "bdd/testdb.db"

int main(void){
  int ret;
  sqlite3* db;
  pbdata_s pbd;
  linprob_s* lp;
  ret = sqlite3_open(DBFILE, &db);

  new_pbdata(db, &pbd);
  
  lp = new_linprob(&pbd);
  free_pbdata(&pbd);
  ret = sqlite3_close(db);
  glp_print_mip(lp->pb, "first_pb.txt");

  free_linprob(lp);
  return EXIT_SUCCESS;
}
