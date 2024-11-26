#ifndef READER_H
#define READER_H
#include "glpapi.h"

#define BUFF_LEN 1024

#define DEF(e, str) e
enum sect_e{
#include "sections.conf"
};
#undef DEF

extern const char* const sections_names[];
extern const char* const stats_symbols[];

int reader(const char* pathname, int* lvl, stat_vect base_stats, 
    int tgt_slots[SLOT_COUNT], double obj_coeff[STATS_COUNT], 
    double bnds[STATS_COUNT], int sign[STATS_COUNT]);

int lock_items_from_file(const char* pathname, linprob_s* lp);

#endif
