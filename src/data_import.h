#ifndef DATA_IMPORT_H
#define DATA_IMPORT_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <sqlite3.h>

#define CREATE_PANOS_SQL "CREATE TABLE IF NOT EXISTS panos" \
	"(id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL, size INTEGER NOT NULL);"

#define CREATE_ITEMS_SQL "CREATE TABLE IF NOT EXISTS items"\
	"(id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL,"\
	"id_pano INTEGER, slot TEXT NOT NULL, category TEXT,"\
	"min_lvl INTEGER, FOREIGN KEY (id_pano) REFERENCES panos(id) "\
	"ON UPDATE CASCADE ON DELETE SET NULL);"

#define CREATE_BONUSES_SQL "CREATE TABLE IF NOT EXISTS bonuses"\
	"(id_pano INTEGER NOT NULL, nb_items INTEGER NOT NULL,"\
	" stat TEXT NOT NULL, value INTEGER NOT NULL,"\
	"UNIQUE (id_pano, nb_item, stat),"\
	"FOREIGN KEY (id_pano) REFERENCES panos(id) "\
	"ON UPDATE CASCADE ON DELETE CASCADE);"

#define CREATE_STATS_SQL "CREATE TABLE IF NOT EXISTS stats(id_item INTEGER, stat TEXT NOT NULL,\
	"minval INTEGER, maxval INTEGER,"\
	"UNIQUE (id_item, stat),"\
	"FOREIGN KEY (id_item) REFERENCES items(id) "\
	"ON UPDATE CASCADE ON DELETE CASCADE);"

#define CREATE_CONSTS_SQL "CREATE TABLE IF NOT EXISTS consts(id_item INTEGER, stat TEXT, "\
	"sign INTEGER, value INTEGER,"\
	"FOREIGN KEY (id_item) REFERRENCES items(id) "\
	"ON UPDATE CASCADE ON DELETE CASCADE);"

#define DROP_ITEMS_SQL "DROP TABLE IF EXISTS items;"
#define DROP_PANOS_SQL "DROP TABLE IF EXISTS panos;"
#define DROP_STATS_SQL "DROP TABLE IF EXISTS stats;"
#define DROP_BONUSES_SQL "DROP TABLE IF EXISTS bonuses;"
#define DROP_CONSTS_SQL "DROP TABLE IF EXISTS consts;"

#define INSERT_PANO "INSERT INTO panos VALUES(?, ?, ?);"
#define INSERT_ITEM "INSERT INTO items VALUES(?, ?, ?, ?, ?, ?);"
#define INSERT_BONUS "INSERT INTO bonus VALUES(?, ?, ?, ?);"
#define INSERT_STAT "INSERT INTO stats VALUES(?, ?, ?, ?);"

int create_tables(sqlite3* db);

int drop_tables(sqlite3* db);

int insert_stat(sqlite3* db, int item_id, char* stat, int value);

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

#endif
