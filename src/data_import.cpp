#include "data_import.h"

#define DEF(e, c, i, d) c
const char* const sql_create[] = {
#include "requests.conf"
};
#undef DEF

#define DEF(e, c, i, d) i
const char* const sql_insert[] = {
#include "requests.conf"
};
#undef DEF

#define DEF(e, c, i, d) d
const char* const sql_drop[] = {
#include "requests.conf"
};
#undef DEF

int create_tables(sqlite3* db){
  char* errmsg;
  for(size_t i = 0; i<SQLC_COUNT; i++){
    if(sqlite3_exec(db, sql_create[i], NULL, NULL, &errmsg)){
      fprintf(stderr, "%s, %s \n", sql_create[i], errmsg);
      sqlite3_free(errmsg);
    }
  }

  return 0;
}

int drop_tables(sqlite3* db){
  char* errmsg;
  for(size_t i = 0; i<SQLC_COUNT; i++){
    if(sqlite3_exec(db, sql_drop[i], NULL, NULL, &errmsg)){
      fprintf(stderr, "%s, %s \n", sql_drop[i], errmsg);
      sqlite3_free(errmsg);
    }
  }

  return 0;
}

int bind_data(sqlite3_stmt* stmt, void* data, char code, int pos){
  if(!data) return sqlite3_bind_null(stmt, pos);
  int val;
  const char* str;
  switch (code) {
    case 'i' :
      val = *(int*)data;
      return sqlite3_bind_int(stmt, pos, val);
      break;
    case 's' :
      str = *(char**)data;
      return sqlite3_bind_text(stmt, pos, str, -1, SQLITE_STATIC);
      break;
    default :
      return 1;
  }

  return 1;
}

int bind_list(sqlite3_stmt* stmt, void* data[], const char* code){
  int ret;
  puts(code);
  putc('\n', stdout);
  for(size_t i = 0; i < strlen(code); i++){
    ret = bind_data(stmt, data[i], code[i], i+1);
    if(ret){
      fprintf(stderr, "bindlist, %s, %d, %s\n", code, i, sqlite3_errstr(ret));
      exit(EXIT_FAILURE);
    }
  }
  return 0;
}

int insert_pano(sqlite3* db, int pano_id, char* name, int size){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(db, sql_insert[SQLC_PANOS], -1, &stmt, NULL);

  void* data[] = {
    (void*) &pano_id,
    (void*) &name,
    (void*) &size
  };

  ret = bind_list(stmt, data, "isi");
  ret = sqlite3_step(stmt);
  ret = sqlite3_finalize(stmt);
  return ret;
}

int insert_item(sqlite3* db, int item_id, char* name, int id_pano, 
    char* slot, char* cat, int min_lvl){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(db, sql_insert[SQLC_ITEMS], -1, &stmt, NULL);

  void* data[] = {
    (void*) &item_id,
    (void*) &name,
    id_pano ? (void*) &id_pano : NULL,
    (void*) &slot,
    cat ? (void*) &cat : NULL,
    (void*) &min_lvl
  };

  ret = bind_list(stmt, data, "isissi");
  ret = sqlite3_step(stmt);
  ret = sqlite3_finalize(stmt);

  return ret;
}

int insert_bonus(sqlite3* db, int pano_id, int nb_item, 
    char* stat, int value){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(db, sql_insert[SQLC_BONUSES], -1, &stmt, NULL);

  void* data[] = {
    (void*) &pano_id,
    (void*) &nb_item,
    (void*) &stat,
    (void*) &value
  };

  ret = bind_list(stmt, data, "iisi");
  ret = sqlite3_step(stmt);
  ret = sqlite3_finalize(stmt);

  return ret;
}

int insert_stat(sqlite3* db, int item_id, char* stat, int value){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(db, sql_insert[SQLC_STATS], -1, &stmt, NULL);

  void* data[] = {
    (void*) &item_id,
    (void*) &stat,
    (void*) &value 
  };

  ret = bind_list(stmt, data, "isi");
  ret = sqlite3_step(stmt);
  ret = sqlite3_finalize(stmt);

  return ret;
}

int insert_spell(sqlite3* db, int spell_id, char* text);

int insert_items_json(sqlite3* db, char* path);

int insert_panos_json(sqlite3* db, char* path);

int insert_weapons_json(sqlite3* db, char* path);

int insert_pets_json(sqlite3* db, char* path);

int insert_mounts_json(sqlite3* db, char* path);

