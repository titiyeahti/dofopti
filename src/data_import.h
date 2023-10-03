#ifndef DATA_IMPORT_H
#define DATA_IMPORT_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <sqlite3.h>

/* enum create insert drop */
#define DEF(e, c, i, d) e

enum tables_e{
#include "requests.conf"
};

#undef DEF

extern const char* const sql_create[];
extern const char* const sql_insert[];
extern const char* const sql_drop[];

int create_tables(sqlite3* db);

int drop_tables(sqlite3* db);

int bind_list(sqlite3_stmt** stmt, void* data[], const char* code);

int insert_pano(sqlite3* db, int pano_id, const char* name);

int insert_const(sqlite3* db, int item_id, const char* stat, 
    int sign, int value);

int insert_item(sqlite3* db, const char* name, int id_pano, 
    const char* slot, const char* cat, int min_lvl);

int insert_bonus(sqlite3* db, int pano_id, int nb_item, 
    const char* stat, int value);

int insert_stat(sqlite3* db, int item_id, const char* stat, 
    int minval, int maxval);

int insert_spell(sqlite3* db, int spell_id, const char* text);

int insert_items_json(sqlite3* db, const char* path);

int insert_panos_json(sqlite3* db, const char* path);

int insert_weapons_json(sqlite3* db, const char* path);

int insert_pets_json(sqlite3* db, const char* path);

int insert_mounts_json(sqlite3* db, const char* path);

#endif
