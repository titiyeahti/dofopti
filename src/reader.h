#ifndef READER_H
#define READER_H
#include "glpapi.h"

#define BUFF_LEN 1024

#define DEF(e, str) e
enum sect_e{
#include "sections.conf"
};
#undef DEF

#define DEF(e, dt, st, str) e
enum elt_e {
#include "elems.conf"
};
#undef DEF

extern const char* const sections_names[];
extern const char* const stats_symbols[];

extern const int elt_do_id[];
extern const int elt_stat_id[];
extern const char* const elt_names[];

int compute_coeff_crit(int elt, int crit, int minv, int maxv, 
    int minc, int maxc, double obj_coeff[STATS_COUNT]);

int compute_coeff_nocri(int elt, int minv, int maxv, 
    double obj_coeff[STATS_COUNT]);

int streamreader(FILE* stream, int* lvl, stat_vect base_stats, 
    int tgt_slots[SLOT_COUNT], double obj_coeff[STATS_COUNT], 
    double bnds[STATS_COUNT], int sign[STATS_COUNT]);

int reader(const char* pathname, int* lvl, stat_vect base_stats, 
    int tgt_slots[SLOT_COUNT], double obj_coeff[STATS_COUNT], 
    double bnds[STATS_COUNT], int sign[STATS_COUNT]);

int lock_items_from_stream(FILE* stream, linprob_s* lp);
int lock_items_from_file(const char* pathname, linprob_s* lp);

#endif
