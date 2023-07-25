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

int bind_list(sqlite3_stmt** stmt, void* data[], char* code);

int insert_stat(sqlite3* db, int item_id, char* stat, int value);

int insert_item(sqlite3* db, int item_id, char* name, int id_pano, 
		char* slot, char* cat, int min_lvl);

int insert_pano(sqlite3* db, int pano_id, char* name, int size);

int insert_bonus(sqlite3* db, int pano_id, int nb_item, 
		char* stat, int value);

int insert_spell(sqlite3* db, int spell_id, char* text);

int insert_items_json(sqlite3* db, char* path);

int insert_panos_json(sqlite3* db, char* path);

int insert_weapons_json(sqlite3* db, char* path);

int insert_pets_json(sqlite3* db, char* path);

int insert_mounts_json(sqlite3* db, char* path);

#endif
