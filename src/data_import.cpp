#include "data_import.h"

/* TODO :
 * store sql requests in separate .dat file for a beter world
 * store problem constants in a separate .dat file for the same reason
 *    each time with enum and char* vec indexed by corresponding enum
 * add abstraction over table insersion and table creation 
 * int create_table(sqlite3* db, char* req);
 * int insert_line(sqlite3* db, char* req, void* argv)
 *    add a way to keep track of positionnal arguments for binds
 *    create an new structure
 *    try not to go mad
 */


int create_tables(sqlite3* db){
	sqlite3_stmt* stmt;
	int ret;
	ret = sqlite3_prepare_v2(db, CREATE_PANOS_SQL, -1, &stmt, NULL);
	if(ret)
		return ret;

	ret = sqlite3_prepare_v2(db, CREATE_ITEMS_SQL, -1, &stmt, NULL);
	if(ret)
		return ret;

	ret = sqlite3_prepare_v2(db, CREATE_BONUSES_SQL, -1, &stmt, NULL);
	if(ret)
		return ret;

	ret = sqlite3_prepare_v2(db, CREATE_STATS_SQL, -1, &stmt, NULL);
	if(ret)
		return ret;

	ret = sqlite3_prepare_v2(db, CREATE_CONSTS_SQL, -1, &stmt, NULL);
	if(ret)
		return ret;

	return 0;
}

int drop_tables(sqlite3* db);

int insert_stat(sqlite3* db, int item_id, char* stat, int value){
	sqlite3_stmt* stmt;

}

int insert_item(sqlite3* db, int item_id, char* name, int id_pano, 
		char* slot, char* cat);

int insert_pano(sqlite3* db, int pano_id, char* name, int size);

int insert_bonus(sqlite3* db, int pano_id, int nb_item, 
		char* stat, int value);

int insert_spell(sqlite3* db, int spell_id, char* text);

int insert_items_json(sqlite3* db, char* path);

int insert_panos_json(sqlite3* db, char* path);

int insert_weapons_json(sqlite3* db, char* path);

int insert_pets_json(sqlite3* db, char* path);

int insert_mounts_json(sqlite3* db, char* path);

