#include "data_import.h"
#define DBFILE "bdd/testdb.db"

int main(){
  int ret;

  sqlite3* db;
  ret = sqlite3_open(DBFILE, &db);
  if(ret){
    fprintf(stderr, "sqlite open\n");
    exit(EXIT_FAILURE);
  }

  drop_tables(db);
  create_tables(db);

  insert_pano(db, 2, "piou rouge", 7);

  insert_item(db, 3, "chapeau piou rouge", 2, "head", NULL, 12);
  
  insert_item(db, 4, "solomonk", 0, "head", NULL, 125);

  insert_item(db, 8, "youyettes", 0, "weapon", "dagues", 156);

  printf("insane\n");

  ret = sqlite3_close(db);
  return EXIT_SUCCESS;
}
