#include "glpapi.h"
#include "reader.h"
#include "linprobjson.h" 
#include "constree.h"

#define DBFILE "skrapipi/pydb2.db"

void intermediate_solutions(glp_tree *T, void *info){
  int i;
  int n;
  double val;
  glp_prob* pb;
  switch (glp_ios_reason(T)){
    case GLP_IBINGO :
      pb = glp_ios_get_prob(T);
      n = glp_get_num_cols(pb);
      for(i=1; i<n+1; i++){
        val = glp_mip_col_val((glp_prob*) info, i);
        if(val>0.5)
          printf("%s : %lf\n", glp_get_col_name((glp_prob*) info, i), val);
      }
      break;
    default :
      break;
  }
  return;
}
      
int solve_linprob_bis(linprob_s* lp){
  // config parm
  glp_iocp parm;
  glp_init_iocp(&parm);
  parm.presolve = GLP_ON;
  parm.cb_func = intermediate_solutions;
  parm.cb_info = lp->pb;

  // solve
  glp_intopt(lp->pb, &parm);
  return 0;
}

int main(int argc, char* argv[]){
  if(argc < 2) exit(1);

  sqlite3* db;
  pbdata_s pbd;
  linprob_s* lp;

  int i, ret, lvl;
  int tgt_slots[SLOT_COUNT] = {1};
  int sign[STATS_COUNT];
  double obj_coeff[STATS_COUNT];
  double bnds[STATS_COUNT];
  stat_vect bs = {0};
  for(i=0; i<SLOT_COUNT; i++) tgt_slots[i] = 1;
  tgt_slots[SLOT_DOFUS] = 6;
  tgt_slots[SLOT_RING] = 2;

  for(i=0; i<STATS_COUNT; i++){
    sign[i] = 0;
    obj_coeff[i] = 0.;
    bnds[i] = 0.;
    bs[i] = 0;
  }

  init();
  ret = sqlite3_open(DBFILE, &db);
  ret = reader(argv[1], &lvl, bs, tgt_slots, obj_coeff, bnds, sign);


  new_pbdata(db, &pbd, bs, tgt_slots, lvl);
  fill_pbd_constraints(db, &pbd);

  lp = new_linprob(&pbd);
  add_all_items_constraints(lp, &pbd);

  set_obj_coeff(lp, obj_coeff);
  const_multi_simple_constraints(lp, bnds, sign);

  lock_items_from_file(argv[1], lp);

  /*
  glp_write_lp(lp->pb, NULL, "outputfiles/cplexout.txt");
  */
  solve_linprob_bis(lp);

  print_linsol(lp, &pbd, stdout);

  if(argc == 3) {
    json_object* root = sol_to_json(lp, &pbd);
    json_object_to_file(argv[2], root);
    json_object_put(root);
  }

  ret = sqlite3_close(db);

  free_pbd_constraints(&pbd);
  free_pbdata(&pbd);

  free_linprob(lp);
  
  glp_free_env();
  return 0;
}
