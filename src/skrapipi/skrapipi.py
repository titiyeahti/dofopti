import requests as rq
import sqlite3 as sql
import csv as csv
import os

dbfile = "pydb5.db"

url= "https://api.beta.dofusdb.fr/"

#--- The pages to fetch from the dofusDb API ---#

def base_url(beta=True):
    if beta : return "https://api.beta.dofusdb.fr/"
    return "https://api.dofusdb.fr/"
def url_effects(pages_read, beta=True):
    return base_url(beta) + "effects?$limit=50&$skip=" +str(pages_read*50)
def url_characteristics(pages_read, beta=True):
    return base_url(beta) + "characteristics?$limit=50&$skip=" +str(pages_read*50)
def url_item_types(pages_read, beta=True):
    return base_url(beta) + "item-types?$limit=50&$skip=" +str(pages_read*50)
def url_item_sets(pages_read, beta=True):
    return base_url(beta) + "item-sets?$limit=50&$skip=" +str(pages_read*50)
def url_items(pages_read, type_id, beta=True):
    return base_url(beta) + "items?$limit=50&typeId=" + str(type_id) + "&$skip=" +str(pages_read*50)
def url_spells(pages_read, class_id, beta=True):
    return base_url(beta) + "spells?typeId=" + str(class_id) + "&$limit=50&$skip=" +str(pages_read*50)
def url_spells_levels(pages_read, class_id, beta=True):
    return base_url(beta) + "spell-levels?spellBreed=" + str(class_id) + "&$limit=50&$skip=" +str(pages_read*50)

tables = [
        "effects", 
        "characteristics", 
        "item_types",
        "item_sets", 
        "items",
        "set_bonuses",
        "item_stats",
        "stat_codes",
        "slot_codes",
        "spell_levels",
        "spells"
        ]

banned_item_type_id = [114, 169, 273, 274, 275, 276, 277, 279, 280]

#Ids des classes dans l'ordre (sram, sacri, forgelance bizarre. 21=sort neutre)
class_ids = [1,2,3,451,5,6,7,8,9,10,580,12,13,14,15,16,17,18,2374,21]

#Les effets qu'on s'autorise à stocker (vol de vie, degat, meilleur element, boost sort)
valid_effects = [91,92,93,94,95,96,97,98,99,100,2822,2828,3293]
sql_create_commands = [
        #--- Crée les tables auxiliaires ---#
        "CREATE TABLE IF NOT EXISTS effects (id INTEGER PRIMARY KEY, description TEXT);", 
        "CREATE TABLE IF NOT EXISTS characteristics (id INTEGER PRIMARY KEY, keyword TEXT);", 
        "CREATE TABLE IF NOT EXISTS item_types (id INTEGER PRIMARY KEY, name TEXT, superTypeId INTEGER, categoryId INTEGER);",
        "CREATE TABLE IF NOT EXISTS item_sets (id INTEGER PRIMARY KEY, name TEXT, namefr TEXT);",
        
        #--- Crée la table principale des items, avec des clés vers les auxis ---#
        "CREATE TABLE IF NOT EXISTS items (id INTEGER PRIMARY KEY, name TEXT, namefr TEXT, level INTEGER, itemTypeId INTEGER, \
        itemSetId INTEGER, criteria TEXT, imgLink TEXT, \
        FOREIGN KEY (itemSetId) REFERENCES item_sets(id) on update cascade on delete cascade , \
        FOREIGN KEY (itemTypeId) REFERENCES item_types(id) on update cascade on delete cascade );",
        
        #--- Crée la table des stats de panoplie, avec des clés vers les auxis ---#
        "CREATE TABLE IF NOT EXISTS set_bonuses (setItemId INTEGER, nbItems INTEGER, carac INTEGER, val INTEGER, \
        FOREIGN KEY (setItemId) REFERENCES item_sets(id) on update cascade on delete cascade , \
        FOREIGN KEY (carac) REFERENCES characteristics(id) on update cascade on delete cascade , \
        UNIQUE (setItemId, nbItems, carac));",
        
        #--- Crée la table des stats de base, avec des clés vers les auxis ---#
        "CREATE TABLE IF NOT EXISTS item_stats (itemId INTEGER, carac INTEGER, minval INTEGER, maxval INTEGER, \
                FOREIGN KEY (itemId) REFERENCES items(id) on update cascade on delete cascade , UNIQUE (itemId, carac) ON CONFLICT IGNORE,\
        FOREIGN KEY (carac) REFERENCES characteristics(id) on update cascade on delete cascade );",
        
        #--- Crée les tables de mapping de stats/slots d'équipements ---#
        "CREATE TABLE IF NOT EXISTS stat_codes (caracId INTEGER, statCode INTEGER);",
        "CREATE TABLE IF NOT EXISTS slot_codes (typeId INTEGER, slotCode INTEGER);",
    
        #--- Crée les tables de spells
        "CREATE TABLE IF NOT EXISTS spells (id INTEGER PRIMARY KEY, name TEXT, classId INTEGER);",
        "CREATE TABLE IF NOT EXISTS spell_levels (levelRequirement INTEGER, effect INTEGER, spell INTEGER, critChance INTEGER,\
            minVal INTEGER, maxVal INTEGER, minCritVal INTEGER, maxCritVal INTEGER,\
            FOREIGN KEY (effect) REFERENCES effects(id) on update cascade on delete cascade,\
            FOREIGN KEY (spell) REFERENCES spells(id) on update cascade on delete cascade);"
        ]

sql_insert_effects_command = "INSERT INTO effects VALUES(?, ?);"
sql_insert_characteristics_command = "INSERT INTO characteristics VALUES(?, ?);"
sql_insert_item_types_command = "INSERT INTO item_types VALUES(?, ?, ?, ?);"
sql_insert_item_sets_command = "INSERT INTO item_sets VALUES(?, ?, ?);"
sql_insert_items_command = "INSERT INTO items (id, name, namefr, level, itemTypeId, itemSetId, criteria, imgLink) VALUES(?, ?, ?, ?, ?, ?, ?, ?);"
sql_insert_set_bonuses_command = "INSERT OR IGNORE INTO set_bonuses (setItemId, nbItems, carac, val) VALUES(?, ?, ?, ?);"
sql_insert_item_stats_command = "INSERT INTO item_stats (itemId, carac, minval, maxval) VALUES(?, ?, ?, ?);"
sql_insert_spell_levels_command = "INSERT INTO spell_levels (levelRequirement, effect, spell, critChance, minVal, maxVal, minCritVal, maxCritVal) VALUES(?, ?, ?, ?, ?, ?, ?, ?);"
sql_insert_spells_command = "INSERT INTO spells (id, name, classId) VALUES(?, ?, ?);"

sql_drop_all_commands = ["DROP TABLE IF EXISTS " + table + ";" for table in tables]
def sql_drop_command(table):
    return "DROP TABLE IF EXISTS " + table + ";"

#import order  : 
#1 meta tables :effects, characteristics, item-types
#2 data tables :items, items set

def run_sql_commands(sql_commands):
    cursor = sql_connection.cursor()
    for sql_command in sql_commands :
        cursor.execute(sql_command)
    sql_connection.commit()

def fetch_spells():
    for class_id in class_ids :
        fetch_spells_for_class(class_id, False)

def fetch_spells_for_class(class_id, force_image):
    i = 0
    cursor = sql_connection.cursor()

    while True :
        print(url_spells(i, class_id))
        r = rq.get(url_spells(i, class_id))
        if r.status_code != 200 :
            break
        js = r.json()
        if not js["data"] :
            break

        for spell in js["data"] :
            name = spell["name"]["fr"]
            id = spell["id"]
            class_id = spell["typeId"]
            img_url = spell['img']

            # 1- Obtient l'icone de l'item et sauvegarde localement.
            file_path = '../../data/spells_img/' + str(id) + '.png'
            if force_image or not os.path.exists(file_path):
                imgdata = rq.get(img_url).content
                os.makedirs(os.path.dirname(file_path), exist_ok=True)
                with open(file_path, 'wb') as handler:
                    handler.write(imgdata)
            
            cursor.execute(sql_insert_spells_command,
                           (
                               id,
                               name,
                               class_id
                           ))
        i += 1
        
    sql_connection.commit()

def fetch_spell_levels():
    for class_id in class_ids :
        fetch_spell_levels_for_class(class_id)
    
def fetch_spell_levels_for_class(class_id):
    i = 0
    cursor = sql_connection.cursor()
    
    while True :
        r = rq.get(url_spells_levels(i, class_id))
        if r.status_code != 200 :
            break
        js = r.json()
        if not js["data"] :
            break
        
        for spell_level in js["data"] :
            normal_array = []
            crit_array = []
            for normal_index in range(len(spell_level["effects"])) :
                effect = spell_level["effects"][normal_index]
                if effect["effectId"] in valid_effects:
                    normal_array.append(effect)
                    
            for crit_index in range(len(spell_level["criticalEffect"])) :
                effect = spell_level["criticalEffect"][crit_index]
                if effect["effectId"] in valid_effects:
                    crit_array.append(effect)
                    
            #--- Fix fleche de merde ---#
            if spell_level["spellId"] == 13062 and spell_level["grade"] == 1:
                normal_array[0]["effectId"]=96
                normal_array[1]["effectId"]=97
            if spell_level["spellId"] == 13062 and spell_level["grade"] == 2:
                normal_array[0]["effectId"]=97
            
            spell_id = spell_level["spellId"]
            crit_chance = spell_level["criticalHitProbability"]
            level_requirement = spell_level["minPlayerLevel"]
            
            for shared_index in range(len(normal_array)):
                effectId = normal_array[shared_index]["effectId"]
                minVal = normal_array[shared_index]["diceNum"]
                maxVal = normal_array[shared_index]["diceSide"]
                minCritVal = crit_array[shared_index]["diceNum"] if crit_chance > 0 else normal_array[shared_index]["diceNum"]
                maxCritVal = crit_array[shared_index]["diceSide"] if crit_chance > 0 else normal_array[shared_index]["diceSide"]
                

                cursor.execute(sql_insert_spell_levels_command,
                               (
                                   level_requirement,
                                   effectId,
                                   spell_id,
                                   crit_chance, 
                                   minVal, 
                                   maxVal, 
                                   minCritVal, 
                                   maxCritVal)
                               )
            #Sanity check des effets normaux X crits
            """
            if crit_chance > 0:
                for shared_index in range(len(normal_array)):
                    if crit_array[shared_index]["effectId"] != normal_array[shared_index]["effectId"]:
                        print(spell_level["spellId"])
            """
        i+=1
        
    sql_connection.commit()
    
            
def fetch_effects() :
    i = 0
    cursor = sql_connection.cursor()
    
    while True :
        r = rq.get(url_effects(i))
        if r.status_code != 200 :
            break
        js = r.json()
        if not js["data"] :
            break

        print("Parsing page " + str(i) + " of effects.")
        for effect_data in js["data"] :
            if effect_data["description"] and "en" in effect_data["description"]:
                cursor.execute(sql_insert_effects_command, (effect_data["id"], effect_data["description"]["en"]))

        i+=1

    sql_connection.commit()


def fetch_characteristics() :
    i = 0
    cursor = sql_connection.cursor()
    while True :
        r = rq.get(url_characteristics(i))
        if r.status_code != 200 :
            break
        js = r.json()
        if not js["data"] :
            break

        print("Parsing page " + str(i) + " of characteristics.")
        for characteristic_data in js["data"] :
            cursor.execute(sql_insert_characteristics_command, (characteristic_data["id"], characteristic_data["keyword"]))

        i+=1
        
    sql_connection.commit()


def fetch_item_types() :
    i = 0
    cursor = sql_connection.cursor()
    while True :
        r = rq.get(url_item_types(i))
        if r.status_code != 200 :
            break
        js = r.json()
        if not js["data"] :
            break

        print("Parsing page " + str(i) + " of item types.")
        for type_data in js["data"] :
            cursor.execute(sql_insert_item_types_command, (type_data["id"], type_data["name"]["en"], type_data["superTypeId"], type_data["categoryId"]))

        i+=1
        
    sql_connection.commit()


def fetch_item_sets() :
    i = 0
    cursor = sql_connection.cursor()
    while True :
        r = rq.get(url_item_sets(i))
        if r.status_code != 200 :
            break
        js = r.json()
        if not js["data"] :
            break

        print("Parsing page " + str(i) + " of item sets.")

        #Pour chaque panoplie, on itère sur les bonus par nombre d'item.
        for item_set in js["data"] :
            cursor.execute(sql_insert_item_sets_command, (item_set["id"], item_set["slug"]["en"], item_set["slug"]["fr"]))
            for effect_index in range(len(item_set["effects"])) :
                for e in item_set["effects"][effect_index] :
                    #On utilise effect_index + 1 pour définir le nombre d'items nécessaires du palier.
                    cursor.execute(sql_insert_set_bonuses_command, (item_set["id"], effect_index + 1, e["characteristic"], e["from"]))

        i+=1
        
    sql_connection.commit()


def fetch_items_of_type(type_id, force_image) :
    i = 0
    cursor = sql_connection.cursor()
    while True :
        r = rq.get(url_items(i, type_id))
        if r.status_code != 200 :
            break
        js = r.json()
        if not js["data"] :
            break

        print("Parsing page " + str(i) + " of items of type " + str(type_id) + ".")
        
        for item in js["data"] :
            # 1- Obtient l'icone de l'item et sauvegarde localement.
            file_path = '../../data/items_img/' + str(item["id"]) + '.png'

            if force_image or not os.path.exists(file_path):
                imgdata = rq.get(item["img"]).content
                os.makedirs(os.path.dirname(file_path), exist_ok=True)
                with open(file_path, 'wb') as handler:
                    handler.write(imgdata)

            # 2- Rafistolage pour obtenir les conditions d'équipement.
            # Certains items utilisent "criterions" au lieu de "criteria".
            # Cela est probablement dû à un changement de l'api non rétroactif.
            criterions = item.get("criterions", item.get("criteria"))

            # 3- Ajout de l'item dans la BDD.
            cursor.execute(
                sql_insert_items_command,
                (
                    item["id"],
                    item["slug"]["en"],
                    item["slug"]["fr"],
                    item["level"],
                    item["typeId"],
                    item["itemSetId"],
                    criterions,
                    item["img"],
                )
            )

            # 4- Ajoute les stats de base de l'item.'
            for item_effect in item["effects"] :
                item_characteristic = item_effect["characteristic"]
                if item_characteristic != -1:
                    cursor.execute(sql_insert_item_stats_command, (item["id"], item_characteristic, item_effect["from"], item_effect["to"]))

        i+=1

    sql_connection.commit()


def fetch_items(force_image = False) :
    cursor = sql_connection.cursor()
    
    # Fetch les items un par un, qui ont une catégorie d'équipement.
    equipment_ids = cursor.execute("select id from item_types where categoryId = 0;")
    for equipment_id in equipment_ids :
        if equipment_id not in banned_item_type_id:
            fetch_items_of_type(equipment_id[0], force_image)


def delete_shit_dot_com() :
    cursor = sql_connection.cursor()

    cursor.execute("delete from item_stats where carac=0;")
    cursor.execute("delete from set_bonuses where carac=0;")
    cursor.execute("update item_stats set "
                "maxval = (case when maxval < minval then maxval else minval end),"
                "maxval = (case when maxval < minval then minval else maxval end);")
    cursor.execute("DELETE FROM items WHERE criteria LIKE ?", ("%~%",))
    cursor.execute("DELETE FROM items WHERE criteria LIKE ?", ("%PX%",))
    cursor.execute("DELETE FROM items WHERE name LIKE ?", ("%(gm%",))
    cursor.execute("UPDATE items SET criteria = ? WHERE itemSetId = ?", ("", 466))
    cursor.execute("delete from item_sets where id = 505;")
    cursor.execute("delete from items where itemSetId = 505;")
    cursor.execute("delete from set_bonuses where setItemId = 505;")

    #supprimer la pano ankarthon et l'amulette chité 
    sql_connection.commit()


def fix_null_maxvals() :
    cursor = sql_connection.cursor()
    cursor.execute("update item_stats set maxval=minval WHERE maxval = 0;")
    sql_connection.commit()


def import_csv_dict_for_mapping(slotcsv, statcsv) :
    cursor = sql_connection.cursor()
    with open(slotcsv, newline='') as csvfile :
        rdr = csv.reader(csvfile, delimiter=',')
        for row in rdr :
            cursor.execute("insert into slot_codes values(?, ?);", (row[0], row[1], ))

    with open(statcsv, newline='') as csvfile :
        rdr = csv.reader(csvfile, delimiter=',')
        for row in rdr :
            cursor.execute("insert into stat_codes values(?, ?);", (row[0], row[1], ))

    sql_connection.commit()


def merge_fetched_data_with_mapping() :
    cursor = sql_connection.cursor()
    
    cursor.execute("alter table items add column slotCode INTEGER;")
    cursor.execute("alter table item_stats add column statCode INTEGER;")
    cursor.execute("alter table set_bonuses add column statCode INTEGER;")
    
    cursor.execute("update items set slotCode = (select slotCode from slot_codes where typeId = itemTypeId);")
    cursor.execute("update item_stats set statCode = (select statCode from stat_codes where caracId = carac);")
    cursor.execute("update set_bonuses set statCode = (select statCode from stat_codes where caracId = carac);")

    sql_connection.commit()

def complete_item_update() :
    #--- Purge la DB si elle existe ---#
    run_sql_commands(sql_drop_all_commands)

    #--- Initialise les différentes tables de la DB ---#
    run_sql_commands(sql_create_commands)
    
    #--- Fetch les différentes pages de l'api dofusDB ---#
    # Obtient les descriptions des effets selon leur id.
    # Affecte seulement la table des "effects".
    fetch_effects()
    # Obtient le keyword des characteristics selon leur id
    # Affecte seuelement la table des "characteristics".
    fetch_characteristics()
    # Obtient les différentes catégories et sous-catégories d'item
    # Affecte seulement la table "item_types".
    fetch_item_types()
    # Obtient les différentes panoplies d'items et leurs bonus par pallier
    # Affecte la table "item_sets" et la table "set_bonuses".
    fetch_item_sets()
    # Obtient la totalité des items équipables.
    # Affecte la table "items" et "item_stats".
    # Dépend de la table "item_sets" qui doit exister précedemment.
    fetch_items()
    
    # --- Convertit les fichiers csv qui mappent les ids de slots/stats en tables de la DB --- #
    import_csv_dict_for_mapping("slot_codes.csv", "stat_codes.csv")
    
    # --- Attribue le mapping aux tables des items et des stats --- #
    # Affecte la table "items" et "item_stats" et "set_bonuses".
    merge_fetched_data_with_mapping()
    
    # --- Fix la fourchette de valeur de l'objet lorsqu'il n'a pas de valeur max (alors min = max). ---#
    # Affecte "item_stats".
    fix_null_maxvals()
    
    # --- Commandes "à la mano" pour réparer les edge cases --- #
    # Affecte "items", "item_stats", "set_bonuses", "item_sets".
    delete_shit_dot_com()

    #--- Préviens l'utilisateur du caractère racite de tolate. [IMPORTANT] ---#
    print("tolate est racite")

if __name__== "__main__" :
    sql_connection = sql.connect(dbfile)
    run_sql_commands([sql_drop_command("spells")])
    run_sql_commands(sql_create_commands)
    fetch_spells()
    fetch_spell_levels()
    
    sql_connection.close()