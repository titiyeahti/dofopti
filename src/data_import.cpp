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

int bind_data(sqlite3_stmt* stmt, void* data, const char code, int pos){
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
  for(size_t i = 0; i < strlen(code); i++){
    ret = bind_data(stmt, data[i], code[i], i+1);
    if(ret){
      fprintf(stderr, "bindlist, %s, %d, %s\n", code, i, sqlite3_errstr(ret));
      exit(EXIT_FAILURE);
    }
  }
  return 0;
}

int insert_pano(sqlite3* db, int pano_id, const char* name){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(db, sql_insert[SQLC_PANOS], -1, &stmt, NULL);

  void* data[] = {
    (void*) &pano_id,
    (void*) &name
  };

  ret = bind_list(stmt, data, "is");
  ret = sqlite3_step(stmt);
  ret = sqlite3_finalize(stmt);
  return ret;
}

int insert_item(sqlite3* db, int item_id, const char* name, int id_pano, 
    const char* slot, const char* cat, int min_lvl){
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
    const char* stat, int value){
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

int insert_stat(sqlite3* db, int item_id, const char* stat, int value){
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

int insert_spell(sqlite3* db, int spell_id, const char* text);

int insert_items_json(sqlite3* db, const char* path){
  int ret;
  std::ifstream ifs(path);
  Json::Reader reader;
  Json::Value root;
  reader.parse(ifs, root);

  for(Json::Value& item : root){
    std::cout << item["name"]["en"] << std::endl;
    Json::Value& stats = item["stats"];

    int id, pano_id, len;
    std::string str = item["dofusID"].asString();
    len = str.strlen();
    id = std::stoi(str.substr(max(len-9,0), 9));

    if(item["setID"].isNull()) pano_id = 0;
    else{
      str = item["setID"].asStrinf();
      len = str.strlen();
      pano_id = std::stoi(str.substr(max(len-9,0), 9));
    }

    ret = insert_item(db, id, item["name"]["en"].asCString(), pano_id,
        item["itemType"].asCString(), item["itemType"].asCString(),
        item["level"].asInt());

    for(Json::Value& stat : stats){
      ret = insert_stat(db, id, stat["stat"].asCString(), 
          stat["maxStat"].asInt());
    }
  }

  return ret;
}

int insert_panos_json(sqlite3* db, const char* path){
  int ret;
  std::ifstream ifs(path);
  Json::Reader reader;
  Json::Value root;
  reader.parse(ifs, root);

  for(Json::Value& pano : root){
    std::cout << pano["name"]["en"] << std::endl;
    Json::Value& bonuses = pano["bonuses"];
    int id, len;
    std::string str = pano["id"].asString();
    len = str.strlen();
    id = std::stoi(str.substr(max(len-9,0),9));

    ret = insert_pano(db, id, pano["name"]["en"].asCString());

    for(std::string& key : bonuses.getMemberNames()){
      for(Json::Value& bonus : bonuses[key]){
        if(bonus["stat"].isNull()) continue;
        ret = insert_bonus(db, id, std::stoi(key),
            bonus["stat"].asCString(), bonus["value"].asInt());
      }
    }
  }

  return ret;
}

int insert_weapons_json(sqlite3* db, const char* path);

int insert_pets_json(sqlite3* db, const char* path);

int insert_mounts_json(sqlite3* db, const char* path);

