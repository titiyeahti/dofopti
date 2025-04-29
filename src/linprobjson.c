#include "linprobjson.h"

json_object* sol_to_json(linprob_s* lp, pbdata_s* pbd){
  int i;
  /*BACK*/
  int slt[10] = {
    SLOT_BACK,
    SLOT_HAT,
    SLOT_BELT,
    SLOT_BOOTS,
    SLOT_AMU,
    SLOT_RING,
    SLOT_DOFUS,
    SLOT_SHIELD,
    SLOT_WEAPON,
    SLOT_PET
  };

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


  json_object_object_add(ret, "AdditionalPoints", points);

  json_object_object_add(ret, "Level", json_object_new_int(pbd->level));
  json_object_object_add(ret, "Result", json_object_new_double(glp_mip_obj_val(lp->pb)));
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
  int real_nb_per_slot[10] = {0};
  int count = 0;
  int ids[16];
  int slot_codes[16];
  int sqret;
  for(i=1; i<pbd->nb_items+1; i++){
    double val = glp_mip_col_val(lp->pb, i);
    if(val>0.5){
      ids[count] = pbd->items_data[i].id;
      slot_codes[count] = pbd->items_data[i].slot_code;
      count++;
    }
    if(val>1.5){
      ids[count] = pbd->items_data[i].id;
      slot_codes[count] = pbd->items_data[i].slot_code;
      count++;
    }
  }


  /* DO YOU WANT TO CREVER */
  int nbi = count;
  for(i=0; i<nbi; i++)
    for(j=0; j<10; j++)
      if(slot_codes[i] == slt[j])
        real_nb_per_slot[j] ++;

  json_object* numpyx = json_object_new_array_ext(17);
  for(j=0; j<10; j++){
    json_object_array_add(numpyx, json_object_new_int(real_nb_per_slot[j]));
  }

  /*for reasons*/
  for(i = 0; i<7; i++) 
    json_object_array_add(numpyx, json_object_new_int(0));

  json_object_object_add(ret, "NumPicks", numpyx);

  json_object* itids = json_object_new_array_ext(17);

  for(j=0; j<10; j++){
    json_object* arr = json_object_new_array_ext(real_nb_per_slot[j]);
    for(i=0; i<nbi; i++)
      if(slot_codes[i] == slt[j])
        json_object_array_add(arr, json_object_new_int(ids[i]));

    json_object_array_add(itids, arr);
  }

  for(i=0; i<7; i++){
    json_object_array_add(itids, json_object_new_array());
  }

  json_object_object_add(ret, "ItemIds", itids);

  json_object* result = json_object_new_array();
  for(i=1; i<lp->n ; i++){
    double val = glp_mip_col_val(lp->pb, i);
    if(val>0.5){
      if (i < pbd->nb_items+1){
        json_object_array_add(result, 
            json_object_new_string(glp_get_col_name(lp->pb, i)));
        if(val>1.5)
          json_object_array_add(result,
              json_object_new_string(glp_get_col_name(lp->pb, i)));
      }
    }
  }

  json_object_object_add(ret, "itemNamesFr", result);

  return ret;
}
