import json as jn
import requests as rq
import sqlite3 as sql
import csv as csv

dbfile = "pydb.db";

cascade = " on update cascade on delete "; 

url= "https://api.beta.dofusdb.fr/";

pages = [
        "effects", 
        "characteristics", 
        "item-types",
        "item-sets",
        "items",
        ];

tables = [
        "effects", 
        "characteristics", 
        "item_types",
        "item_sets", 
        "items",
        "set_bonuses",
        "item_stats",
        "stat_codes",
        "slot_codes"
        ];

sql_create = [
        "CREATE TABLE IF NOT EXISTS effects (id INTEGER PRIMARY KEY, description TEXT);", 
        "CREATE TABLE IF NOT EXISTS characteristics (id INTEGER PRIMARY KEY, keyword TEXT);", 
        "CREATE TABLE IF NOT EXISTS item_types (id INTEGER PRIMARY KEY, name TEXT, superTypeId INTEGER, categoryId INTEGER);",
        "CREATE TABLE IF NOT EXISTS item_sets (id INTEGER PRIMAEY KEY, name TEXT);",
        "CREATE TABLE IF NOT EXISTS items (id INTEGER PRIMARY KEY, name TEXT, level INTEGER, itemTypeId INTEGER, itemSetId INTEGER, criteria TEXT, \
        FOREIGN KEY (itemSetId) REFERENCES item_sets(id), \
        FOREIGN KEY (itemTypeId) REFERENCES item_types(id));",
        "CREATE TABLE IF NOT EXISTS set_bonuses (setItemId INTEGER, nbItems INTEGER, carac INTEGER, val INTEGER, \
        FOREIGN KEY (setItemId) REFERENCES set_items(id), \
        FOREIGN KEY (carac) REFERENCES characteristics(id), \
        UNIQUE (setItemId, nbItems, carac));",
        "CREATE TABLE IF NOT EXISTS item_stats (itemId INTEGER, carac INTEGER, minval INTEGER, maxval INTEGER, \
        FOREIGN KEY (itemId) REFERENCES items(id), UNIQUE (itemId, carac)\
        FOREIGN KEY (carac) REFERENCES characteristics(id));",
        "CREATE TABLE IF NOT EXISTS stat_codes (caracId INTEGER, statCode INTEGER);",
        "CREATE TABLE IF NOT EXISTS slot_codes (typeId INTEGER, slotCode INTEGER);"
        ];

sql_insert = [
        "INSERT INTO effects VALUES(?, ?);",
        "INSERT INTO characteristics VALUES(?, ?);",
        "INSERT INTO item_types VALUES(?, ?, ?, ?);",
        "INSERT INTO item_sets VALUES(?, ?);",
        "INSERT INTO items VALUES(?, ?, ?, ?, ?, ?);",
        "INSERT OR IGNORE INTO set_bonuses VALUES(?, ?, ?, ?);",
        "INSERT INTO item_stats VALUES(?, ?, ?, ?);"
        ];

sql_drop = ["DROP TABLE IF EXISTS " + table + ";" for table in tables];

#import order  : 
#1 meta tables :effects, characteristics, item-types
#2 data tables :items, items set

def create_tables(con) :
    cur = con.cursor();
    for stmt in sql_create :
        cur.execute(stmt);
    con.commit();

def drop_tables(con) :
    cur = con.cursor();
    for stmt in sql_drop :
        cur.execute(stmt);
    con.commit();

def fetch_effects(con) :
    i = 0;
    cur = con.cursor();
    while True :
        r = rq.get(url+pages[0]+"?$limit=50&$skip="+str(i*50));
        if r.status_code != 200 :
            break;

        js = r.json();
        if not js["data"] :
            break;

        for effect in js["data"] :
            if len(effect["description"]) > 5 :
                cur.execute(sql_insert[0], (effect["id"], effect["description"]["en"]));

        i+=1;

    con.commit();

def fetch_characteristics(con) :
    i = 0;
    cur = con.cursor();
    while True :
        r = rq.get(url+pages[1]+"?$limit=50&$skip="+str(i*50));
        if r.status_code != 200 :
            break;

        js = r.json();
        if not js["data"] :
            break;

        for char in js["data"] :
            cur.execute(sql_insert[1], (char["id"], char["keyword"]));

        i+=1;

    con.commit();

def fetch_item_types(con) :
    i = 0;
    cur = con.cursor();
    while True :
        r = rq.get(url+pages[2]+"?$limit=50&$skip="+str(i*50));
        if r.status_code != 200 :
            break;

        js = r.json();
        if not js["data"] :
            break;

        for typ in js["data"] :
            cur.execute(sql_insert[2], (typ["id"], typ["name"]["en"], typ["superTypeId"], typ["categoryId"]));

        i+=1;

    con.commit();

def fetch_item_sets(con) :
    i = 0;
    cur = con.cursor();
    while True :
        print(i);
        r = rq.get(url+pages[3]+"?$limit=50&$skip="+str(i*50));
        if r.status_code != 200 :
            break;

        js = r.json();
        if not js["data"] :
            break;

        for iset in js["data"] :
            print(iset["slug"]["en"], iset["id"]);
            cur.execute(sql_insert[3], (iset["id"], iset["slug"]["en"]));
            for j in range(len(iset["effects"])) :
                for e in iset["effects"][j] :
                    cur.execute(sql_insert[5], (iset["id"], j+1, e["characteristic"], e["from"]));


        i+=1;

    con.commit();

def fetch_items_of_type(con, typeId) :
    i = 0;
    cur = con.cursor();
    while True :
        r = rq.get(url+pages[4]+"?&$limit=50&$skip="+str(i*50)+"&typeId="+str(typeId));
        if r.status_code != 200 :
            break;

        js = r.json();
        if not js["data"] :
            break;

        for item in js["data"] :
            print(item["slug"]["en"])
            cur.execute(sql_insert[4], (item["id"], item["slug"]["en"], item["level"], 
                                        item["typeId"], item["itemSetId"], item["criteria"]));
            for e in item["effects"] :
                cr = e["characteristic"]
                if cr != -1:
                    cur.execute(sql_insert[6], (item["id"], cr, e["from"], e["to"]));


        i+=1;

    con.commit();

def fetch_items(con) :
    cur = con.cursor();
    res = cur.execute("select id from item_types where categoryId = 0;");
    for ids in res :
        print(ids);
        fetch_items_of_type(con, ids[0]);


def delete_shit_dot_com(con) :
    l=[114, 169, 273, 274, 275, 276, 277, 279, 280];
    sqlshit = "delete from items where itemTypeId = ?;";
    cur = con.cursor();
    for e in l:
        cur.execute(sqlshit, (e, ));

    cur.execute("delete from item_stats where carac=0;");
    cur.execute("delete from set_bonuses where carac=0;");
    cur.execute("update item_stats set maxval = minval, minval = maxval where maxval < minval;");
    cur.execute("delete from items where criteria like \"%~%\";");
    #supprimer la pano ankarthon et l'amulette chité 
    con.commit();

def fix_null_maxvals(con) :
    cur = con.cursor();
    cur.execute("update item_stats set maxval=minval WHERE maxval = 0;");
    con.commit();

def import_csv_dict(con, slotcsv, statcsv) :
    cur = con.cursor();
    with open(slotcsv, newline='') as csvfile :
        rdr = csv.reader(csvfile, delimiter=',');
        for row in rdr :
            cur.execute("insert into slot_codes values(?, ?);", (row[0], row[1], ));

    with open(statcsv, newline='') as csvfile :
        rdr = csv.reader(csvfile, delimiter=',');
        for row in rdr :
            cur.execute("insert into stat_codes values(?, ?);", (row[0], row[1], ));

    con.commit();


def add_cools(con) :
    cur = con.cursor();
    #new columns
    cur.execute("alter table items add column slotCode INTEGER;");
    cur.execute("alter table item_stats add column statCode INTEGER;");
    cur.execute("alter table set_bonuses add column statCode INTEGER;");
    #uptade
    cur.execute("update items set slotCode = (select slotCode from slot_codes where typeId = itemTypeId);");
    cur.execute("update item_stats set statCode = (select statCode from stat_codes where caracId = carac);");
    cur.execute("update set_bonuses set statCode = (select statCode from stat_codes where caracId = carac);");

    con.commit();


if __name__== "__main__" :
    """
    i = 300;
    while True :
        r = rq.get(url+"items?$limit=50&$skip="+str(i*50));
        if(r.status_code != 200) :
            break;


        js = r.json()
        for item in js["data"] :
            print(item["name"]["fr"]);
        i += 1;
    """
    con = sql.connect(dbfile);
    cur = con.cursor();
    cur.execute(sql_drop[8]);
    cur.execute(sql_drop[7]);
    create_tables(con);
    import_csv_dict(con, "slot_codes.csv", "stat_codes.csv");
    add_cools(con);
    fix_null_maxvals(con);
    #fetch_effects(con);
    #fetch_characteristics(con);
    #fetch_item_types(con);
    #con.execute(sql_drop[4]);
    #create_tables(con);
    #fetch_item_sets(con);
    #fetch_items(con);
    #delete_shit_dot_com(con);
    con.close();

    print("tolate est racite");

