#include "data_import.h"
#define DBFILE "bdd/testdb.db"
#define PANOSJSON "../data/sets.json"
#define ITEMSJSON "../data/items.json"

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

  insert_panos_json(db, PANOSJSON);
  insert_items_json(db, ITEMSJSON);

  ret = sqlite3_close(db);
  return EXIT_SUCCESS;
}
