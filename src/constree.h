#ifndef CONSTREE_H
#define CONSTREE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glpapi.h"

typedef struct constree{
  int stat;
  char sign;
  int val;
  struct constree * lt;
  struct constree * rt;
} constree_s;

constree_s* new_constree(int stat, char sign, int val);

constree_s* constree_from_str(const char* str);

int symbol_to_stat(char symbol[3]);

constree_s * cleantree(constree_s* ct);

void printree(constree_s* ct);

void free_constree(constree_s* ct);


#endif
