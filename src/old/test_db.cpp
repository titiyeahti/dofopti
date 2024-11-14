#include "data_import.h"
#define DBFILE "bdd/testdb.db"
#define PANOSJSON "../data/sets.json"
#define ITEMSJSON "../data/items.json"
#define VOLKSJSON "../data/rhineetles.json"
#define PETSJSON "../data/pets.json"
#define MOUNTSJSON "../data/mounts.json"
#define WEAPONSJSON "../data/weapons.json"

int main(){
  int ret;

  sqlite3* db;
  ret = sqlite3_open(DBFILE, &db);
  if(ret){
    fprintf(stderr, "sqlite open\n");
    exit(EXIT_FAILURE);
  }

  init_db(db);

  insert_panos_json(db, PANOSJSON);
  insert_items_json(db, ITEMSJSON);
  insert_items_json(db, VOLKSJSON);
  insert_items_json(db, MOUNTSJSON);
  insert_items_json(db, PETSJSON);
  insert_items_json(db, WEAPONSJSON);

  ret = sqlite3_close(db);
  return EXIT_SUCCESS;
}
