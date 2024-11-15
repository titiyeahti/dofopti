#include "glpapi.h"
#define DBFILE "bdd/testdb.db"

int main(void){
  int ret;
  sqlite3* db;
  ret = sqlite3_open(DBFILE, &db);

  pbdata_s pbd;
  new_pbdata(db, &pbd);
  print_pbdata(&pbd);
  free_pbdata(&pbd);

  ret = sqlite3_close(db);

  return EXIT_SUCCESS;
}
