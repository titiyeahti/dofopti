#ifndef CONSTREE_H
#define CONSTREE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glpapi.h"
#include "data_types.h"

#define BIG_M 65536.
#define MAX_DEPTH 10

constree_s* new_leaf(int stat, int sign, int val);

constree_s* new_braces(constree_s** ct);

constree_s* new_node(int t, constree_s** lm, constree_s** rm);

constree_s* constree_from_str(const char* str);

int symbol_to_stat(char symbol[3]);

int eval(constree_s* ct);

constree_s* cutting(constree_s* ct);

int add_item_constraint(int item_id, linprob_s* lp, pbdata_s* pbd);

int add_all_items_constraints(linprob_s* lp, pbdata_s* pbd);

int fill_pbd_constraints(sqlite3* db, pbdata_s* pbd);

void free_pbd_constraints(pbdata_s* pbd);

void print_constree(constree_s* ct);

void free_constree(constree_s** ct);


/*

int symbol_to_stat(char symbol[3]);

void cleantree(constree_s** ct);

void printree(constree_s* ct);
*/



#endif
