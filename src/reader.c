#include "reader.h"

#define DEF(e, str) str
const char* const sections_names[] = {
#include "sections.conf"
};
#undef DEF

#define DEF(e, str, s) s
const char * const stats_symbols[] = {
#include "stats.conf"
};
#undef DEF

#define DEF(e, dt, st, str) dt
const int elt_do_id[ELEM_COUNT] = {
#include "elems.conf"
};
#undef DEF

#define DEF(e, dt, st, str) st
const int elt_stat_id[] = {
#include "elems.conf"
};
#undef DEF

#define DEF(e, dt, st, str) str
const char* const elt_names[] = {
#include "elems.conf"
};
#undef DEF

#define EFF_CRIT(c) ((c) < 0 ? 0 : ((c) > 100 ? 100 : (c) ))

int compute_coeff_crit(int elt, int crit, int minv, int maxv, 
    int minc, int maxc, double obj_coeff[STATS_COUNT]){
  double val = (double) (EFF_CRIT(crit) * (minc + maxc) + 
      (100 - EFF_CRIT(crit)) * (minv + maxv)) / (200.*100.);

  obj_coeff[elt_stat_id[elt]] += val;
  obj_coeff[PUI] += val;
  obj_coeff[DO] += 1.;
  obj_coeff[elt_do_id[elt]] += 1.;

  obj_coeff[DO_CRIT] += (double) EFF_CRIT(crit) / (100.);

  return 0;
}


int compute_coeff_nocri(int elt, int minv, int maxv, 
    double obj_coeff[STATS_COUNT]){
  double val = (double) (minv + maxv) / (2.*100.);
  obj_coeff[elt_stat_id[elt]] += val;
  obj_coeff[PUI] += val;
  obj_coeff[DO] += 1.;
  obj_coeff[elt_do_id[elt]] += 1.;

  return 0;
}


int streamreader(FILE* stream, int* lvl, stat_vect base_stats, 
    int tgt_slots[SLOT_COUNT], double obj_coeff[STATS_COUNT], 
    double bnds[STATS_COUNT], int sign[STATS_COUNT], 
    short_word locked_items[MAX_LOCKS], int* nb_locks){

  int ret;
  int i, sect, crit_flag, elt;
  char buffer[BUFF_LEN];

  *nb_locks = 0;
  crit_flag = 0;
  sect = -1;
  elt = -1;
  while(fgets(buffer, BUFF_LEN - 1, stream)){
    if(buffer[0] == '%') continue;

    if(buffer[0] == '#'){
      buffer[strlen(buffer)-1] = '\0';
      for(i = 0; i<SECT_COUNT; i++)
        if(!strcmp(sections_names[i], (buffer+1))){
          sect = i;
          break;
        }

      continue;
    }

    if(sect < 0){
      fprintf(stderr, "incorrect config file\n");
      exit(1);
    }

    char str[48];
    char sig[3];
    int val;
    double dval;
    /* TODO verify if fgets keeps '\n' */
    switch (sect) {
      case SECT_LVL :
        if(sscanf(buffer, "%d\n", lvl)!=1){
          fprintf(stderr, "Section lvl incorrecte\n");
          exit(1);
        }
        break;

      case SECT_CRIT :
        char* endptr;
        long crit = strtol(buffer, &endptr, 10);
        if(endptr != buffer){
          crit_flag = 1;
          sign[CRIT] = -1; bnds[CRIT] = crit;
        }
        break;

      case SECT_SLOTS :
        if(sscanf(buffer, "%s %d\n", str, &val)!= 2){
          fprintf(stderr, "section slots incorrecte\n");
          exit(1);
        }

        for(i=0; i<SLOT_COUNT; i++){
          if(!strcmp(slots_names[i], str)){
            tgt_slots[i] = val;
            break;
          }
        }
        break;

      case SECT_BS :
        if(sscanf(buffer, "%s %d\n", str, &val)!= 2){
          fprintf(stderr, "incorrect base stats section\n");
          exit(1);
        }

        for(i=0; i<STATS_COUNT; i++){
          if(!strcmp(stats_symbols[i], str)){
            base_stats[i] = val;
            break;
          }
        }
        break;

      case SECT_OBJ :
        if(sscanf(buffer, "%s %lf\n", str, &dval)!= 2){
          fprintf(stderr, "incorrect base stats section\n");
          exit(1);
        }

        for(i=0; i<STATS_COUNT; i++){
          if(!strcmp(stats_symbols[i], str)){
            obj_coeff[i] = dval;
            break;
          }
        }
        break;

      case SECT_DMG :
        int crit_rate, minv, maxv, minc, maxc;
        if(sscanf(buffer, "%s %d %d %d %d %d\n", 
              str, &crit_rate, &minv, &maxv, &minc, &maxc) != 6){
          fprintf(stderr, "linea de damagos incorecta\n");
          exit(1);
        }

        for(i=0; i<ELEM_COUNT; i++){
          if(!strcmp(str, elt_names[i])){
            elt = i;
            break;
          }
        }

        if (i==ELEM_COUNT) break;
        if (elt==POUSSEE){
          obj_coeff[DO_POU] += minv/4.;
          break;
        }

        crit_flag && crit ? 
          compute_coeff_crit(elt, crit_rate+bnds[CRIT], 
              minv, maxv, minc, maxc, obj_coeff):
          compute_coeff_nocri(elt, minv, maxv, obj_coeff);

        break;

      case SECT_CONSTS :
        if(sscanf(buffer, "%s %s %lf\n", str, sig, &dval)!= 3){
          fprintf(stderr, "section de restriction incorrecta\n");
          exit(1);
        }

        for(i=0; i<STATS_COUNT; i++){
          if(!strcmp(stats_symbols[i], str)){
            bnds[i] = dval;
            switch(sig[0]) {
              case '=' :
                sign[i] = 0;
                break;
              case '<' :
                sign[i] = 1;
                break;
              case '>' :
                sign[i] = -1;
                break;
              default :
                fprintf(stderr, "problème au niveau d'un signe de contrainte\n");
                exit(1);
            }
            break;
          }
        }
        break;

      case SECT_ITEMS :
        if(buffer[strlen(buffer)-1] == '\n')
          buffer[strlen(buffer)-1] = '\0';


        strcpy(locked_items[*nb_locks], buffer);
        *nb_locks += 1;

        break;
        
      default :
        fprintf(stderr, "étrange, cela n'est pas sensé se produire, manque de pot\n");
        exit(1);
    }
  }

  return 0;

}

int reader(const char* pathname, int* lvl, stat_vect base_stats, 
    int tgt_slots[SLOT_COUNT], double obj_coeff[STATS_COUNT], 
    double bnds[STATS_COUNT], int sign[STATS_COUNT], 
    short_word locked_items[MAX_LOCKS], int* nb_locks){
  int ret;
  FILE* stream = fopen(pathname, "r");
  if (!stream) {
    fprintf(stderr, "cannot open file for reading\n");
    exit(1);
  }

  ret = streamreader(stream, lvl, base_stats, tgt_slots, obj_coeff, bnds, sign,
      locked_items, nb_locks);


  fclose(stream);

  return 0;
}

/*
int lock_items_from_stream(FILE* stream, linprob_s* lp){
  int i, sect;
  char buffer[BUFF_LEN];

  sect = -1;
  while(fgets(buffer, BUFF_LEN - 1, stream)){
    if(buffer[0] == '#'){
      buffer[strlen(buffer)-1] = '\0';
      for(i = 0; i<SECT_COUNT; i++)
        if(!strcmp(sections_names[i], buffer+1)){
          sect = i;
          break;
        }

      continue;
    }

    if(sect < 0){
      fprintf(stderr, "incorrect config file\n");
      exit(1);
    }

    if(sect != SECT_ITEMS) continue;

    if(buffer[strlen(buffer)-1] == '\n')
      buffer[strlen(buffer)-1] = '\0';

    if(buffer[0] == '-'){
      const_lock_out_item(lp, (const char*) buffer+1);
    }
    if(buffer[0] == '+'){
      const_lock_in_item(lp, (const char*) buffer+1);
    }
  }

  return 0;
}

int lock_items_from_file(const char* pathname, linprob_s* lp){
  char buffer[BUFF_LEN];
  FILE* stream = fopen(pathname, "r");
  if (!stream) {
    fprintf(stderr, "cannot open file for reading\n");
    exit(1);
  }

  lock_items_from_stream(stream, lp);

  fclose(stream);
  return 0;
}
*/

int lock_items_from_array(short_word locked_items[MAX_LOCKS], int nb_locks,
    linprob_s* lp){
  int i;
  for(i=0; i<nb_locks; i++){
    if(locked_items[i][0] == '-'){
      const_lock_out_item(lp, (const char*) locked_items[i]+1);
    }
    if(locked_items[i][0] == '+'){
      const_lock_in_item(lp, (const char*) locked_items[i]+1);
    }
  }

  return 0;
}
