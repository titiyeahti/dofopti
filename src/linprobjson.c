#include "linprobjson.h"

json_object* sol_to_json(linprob_s* lp, pbdata_s* pbd, sqlite3* db){
  int i;
  json_object* ret = json_object_new_object();

  json_object* bz_carac = json_object_new_array_ext(12);
  json_object_array_add(bz_carac, json_object_new_int(pbd->base_stats[VITA]));
  json_object_array_add(bz_carac, json_object_new_int(pbd->base_stats[SA]));
  json_object_array_add(bz_carac, json_object_new_int(pbd->base_stats[FOR]));
  json_object_array_add(bz_carac, json_object_new_int(pbd->base_stats[INT]));
  json_object_array_add(bz_carac, json_object_new_int(pbd->base_stats[CHA]));
  json_object_array_add(bz_carac, json_object_new_int(pbd->base_stats[AGI]));
  /*PA PM*/
  json_object_array_add(bz_carac, json_object_new_int(pbd->level >= 100 ? 7 : 6));
  json_object_array_add(bz_carac, json_object_new_int(3));
  json_object_array_add(bz_carac, json_object_new_int(0));
  json_object_array_add(bz_carac, json_object_new_int(pbd->base_stats[PP]));
  json_object_array_add(bz_carac, json_object_new_int(0));
  /*invo*/
  json_object_array_add(bz_carac, json_object_new_int(pbd->base_stats[INVO]));
  
  json_object_object_add(ret, "BaseCaracs", bz_carac);

  json_object* points = json_object_new_array_ext(6);
  /*TODO*/
  /*int stats_codes[6] = {AGI, CHA, INT, FOR, VITA, SA};*/
  int vals[6] = {0};
  int j;
  int id = pbd->nb_items + pbd->nb_bonuses + 1;
  for(i=0; i<NB_ELEMS; i++){
    vals[i] = 0; 
    for(j = 0; j<NB_COLS_PER_ELEM; j++){
      vals[i] += (int) glp_mip_col_val(lp->pb, id);
      id++;
    }
  }

  vals[4] = (int) glp_mip_col_val(lp->pb, id); id++;
  vals[5] = (int) glp_mip_col_val(lp->pb, id);

  json_object_array_add(points, json_object_new_int(vals[4]));
  json_object_array_add(points, json_object_new_int(vals[5]));
  json_object_array_add(points, json_object_new_int(vals[3]));
  json_object_array_add(points, json_object_new_int(vals[2]));
  json_object_array_add(points, json_object_new_int(vals[1]));
  json_object_array_add(points, json_object_new_int(vals[0]));


  json_object_object_add(ret, "AddtionalPoints", points);

  json_object_object_add(ret, "Level", json_object_new_int(pbd->level));
  /*boolean stuff*/
  json_object_object_add(ret, "Poney", json_object_new_boolean(0));
  json_object_object_add(ret, "PuissanceCarac", json_object_new_boolean(0));
  json_object_object_add(ret, "AllowDamage", json_object_new_boolean(0));
  json_object_object_add(ret, "ExoPA", 
    json_object_new_boolean(pbd->base_stats[PA] - (pbd->level >= 100 ? 7 : 6)));
  json_object_object_add(ret, "ExoPM", 
    json_object_new_boolean(pbd->base_stats[PM] - 3));
  json_object_object_add(ret, "ExoPO", 
    json_object_new_boolean(pbd->base_stats[PO]));

  /* cape, chapeau, ceinture, bottes, collier, anneau anneau, dofus, bouclier, arme, pet
   * + sept slots vides*/
  
  json_object* numpyx = json_object_new_array_ext(17);
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_BACK]));
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_HAT]));
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_BELT]));
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_BOOTS]));
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_AMU]));
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_RING]));
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_DOFUS]));
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_SHIELD]));
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_WEAPON]));
  json_object_array_add(numpyx, json_object_new_int(pbd->targeted_slots[SLOT_PET]));

  /*for reasons*/
  for(i = 0; i<7; i++) 
    json_object_array_add(numpyx, json_object_new_int(0));

  json_object_object_add(ret, "NumPicks", numpyx);

  sqlite3_stmt* stmt;

  int nbi = 0;
  for(i=0; i<SLOT_COUNT; i++){
    nbi+= pbd->targeted_slots[i];
  }

  int count = 0;
  int ids[nbi];
  int slot_codes[nbi];
  int sqret;
  for(i=1; i<pbd->nb_items+1; i++){
    double val = glp_mip_col_val(lp->pb, i);
    if(val>0.5){
      sqret = sqlite3_prepare_v2(db, 
          "select id, slotCode from items where name = ? limit 1;", -1, &stmt, NULL);
      sqret = sqlite3_bind_text(stmt, 1, glp_get_col_name(lp->pb, i), -1, NULL);
      sqret = sqlite3_step(stmt);
      ids[count] = sqlite3_column_int(stmt, 0);
      slot_codes[count] = sqlite3_column_int(stmt, 1);
      sqret = sqlite3_finalize(stmt);
      count++;

      if(count == nbi)
        break;
    }
  }



  /* DO YOU WANT TO CREVER */
  json_object* itids = json_object_new_array_ext(17);
  json_object* back = json_object_new_array_ext(pbd->targeted_slots[SLOT_BACK]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_BACK)
      json_object_array_add(back, json_object_new_int(ids[i]));

  json_object_array_add(itids, back);

  json_object* hat = json_object_new_array_ext(pbd->targeted_slots[SLOT_HAT]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_HAT)
      json_object_array_add(hat, json_object_new_int(ids[i]));

  json_object_array_add(itids, hat);

  json_object* belt = json_object_new_array_ext(pbd->targeted_slots[SLOT_BELT]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_BELT)
      json_object_array_add(belt, json_object_new_int(ids[i]));

  json_object_array_add(itids, belt);

  json_object* boots = json_object_new_array_ext(pbd->targeted_slots[SLOT_BOOTS]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_BOOTS)
      json_object_array_add(boots, json_object_new_int(ids[i]));

  json_object_array_add(itids, boots);

  json_object* amu = json_object_new_array_ext(pbd->targeted_slots[SLOT_AMU]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_AMU)
      json_object_array_add(amu, json_object_new_int(ids[i]));

  json_object_array_add(itids, amu);

  json_object* ring = json_object_new_array_ext(pbd->targeted_slots[SLOT_RING]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_RING)
      json_object_array_add(ring, json_object_new_int(ids[i]));

  json_object_array_add(itids, ring);

  json_object* dofus = json_object_new_array_ext(pbd->targeted_slots[SLOT_DOFUS]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_DOFUS)
      json_object_array_add(dofus, json_object_new_int(ids[i]));

  json_object_array_add(itids, dofus);

  json_object* shield = json_object_new_array_ext(pbd->targeted_slots[SLOT_SHIELD]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_SHIELD)
      json_object_array_add(shield, json_object_new_int(ids[i]));

  json_object_array_add(itids, shield);

  json_object* weapon = json_object_new_array_ext(pbd->targeted_slots[SLOT_WEAPON]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_WEAPON)
      json_object_array_add(weapon, json_object_new_int(ids[i]));

  json_object_array_add(itids, weapon);

  json_object* pet = json_object_new_array_ext(pbd->targeted_slots[SLOT_PET]);
  for(i=0; i<nbi; i++)
    if(slot_codes[i] == SLOT_PET)
      json_object_array_add(pet, json_object_new_int(ids[i]));

  json_object_array_add(itids, pet);

  for(i=0; i<7; i++){
    json_object_array_add(itids, json_object_new_array());
  }

  json_object_object_add(ret, "ItemIds", itids);


  return ret;
}
