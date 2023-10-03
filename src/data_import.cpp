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

int insert_const(sqlite3* db, int item_id, const char* stat, 
    int sign, int value){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(db, sql_insert[SQLC_CONSTS], -1, &stmt, NULL);

  void* data[] = {
    (void*) &item_id,
    (void*) &stat,
    (void*) &sign,
    (void*) &value,
  };

  ret = bind_list(stmt, data, "isii");
  ret = sqlite3_step(stmt);
  ret = sqlite3_finalize(stmt);

  return ret;
}

int insert_item(sqlite3* db, const char* name, int id_pano, 
    const char* slot, const char* cat, int min_lvl){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(db, sql_insert[SQLC_ITEMS], -1, &stmt, NULL);

  void* data[] = {
    (void*) &name,
    id_pano ? (void*) &id_pano : NULL,
    (void*) &slot,
    cat ? (void*) &cat : NULL,
    (void*) &min_lvl
  };

  ret = bind_list(stmt, data, "sissi");
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

int insert_stat(sqlite3* db, int item_id, const char* stat, 
    int minval, int maxval){
  int ret;
  sqlite3_stmt* stmt;
  ret = sqlite3_prepare(db, sql_insert[SQLC_STATS], -1, &stmt, NULL);

  void* data[] = {
    (void*) &item_id,
    (void*) &stat,
    (void*) &minval,
    (void*) &maxval
  };

  ret = bind_list(stmt, data, "isii");
  ret = sqlite3_step(stmt);
  ret = sqlite3_finalize(stmt);

  return ret;
}

int insert_spell(sqlite3* db, int spell_id, const char* text);

int yetistoi(std::string str){
  int len = (int) str.length();
  return std::stoi(str.substr(std::max(len-9,0), 9));
}

int insert_items_json(sqlite3* db, const char* path){
  int ret;
  std::ifstream ifs(path);
  Json::Reader reader;
  Json::Value root;
  reader.parse(ifs, root);

  ret = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
  for(Json::Value& item : root){
    Json::Value& stats = item["stats"];
    Json::Value& consts = item["conditions"]["conditions"]["and"];

    int id, pano_id;

    if(item["setID"].isNull()) pano_id = 0;
    else
      pano_id = yetistoi(item["setID"].asString());

    ret = insert_item(db, item["name"]["en"].asCString(), pano_id,
        item["itemType"].asCString(), item["itemType"].asCString(),
        item["level"].asInt());

    id = (int) sqlite3_last_insert_rowid(db);

    for(Json::Value& stat : stats){
      ret = insert_stat(db, id, stat["stat"].asCString(), 
          stat["minStat"].asInt(), stat["maxStat"].asInt());
    }

    for(Json::Value& cond : consts){
      std::cout << item["name"]["en"] << std::endl;
      int sign = 0;
      switch (cond["operator"].asCString()[0]) {
        case '<' :
          sign = -1;
          break;
        case '>':
          sign = 1;
          break;
        case '=' :
          sign = 0;
          break;
        default :
          sign = 2;
      }
      
      ret = insert_const(db, id, cond["stat"].asCString(), sign, 
          cond["value"].asInt());
    }
  }

  ret = sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
  return ret;
}

int insert_panos_json(sqlite3* db, const char* path){
  int ret;
  std::ifstream ifs(path);
  Json::Reader reader;
  Json::Value root;
  reader.parse(ifs, root);

  ret = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
  for(Json::Value& pano : root){
    std::cout << pano["name"]["en"] << std::endl;
    Json::Value& bonuses = pano["bonuses"];
    int id;
    id = yetistoi(pano["id"].asString());

    ret = insert_pano(db, id, pano["name"]["en"].asCString());

    for(std::string& key : bonuses.getMemberNames()){
      for(Json::Value& bonus : bonuses[key]){
        if(bonus["stat"].isNull()) continue;
        ret = insert_bonus(db, id, std::stoi(key),
            bonus["stat"].asCString(), bonus["value"].asInt());
      }
    }
  }

  ret = sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
  return ret;
}

int insert_weapons_json(sqlite3* db, const char* path);

int insert_pets_json(sqlite3* db, const char* path);

int insert_mounts_json(sqlite3* db, const char* path);

