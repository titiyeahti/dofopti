#ifndef CONSTREE_H
#define CONSTREE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glpapi.h"

typedef struct constree{
  enum Type {LEAF, BRACES, OR, AND} t;
  union {
    struct {
      int stat;
      int sign;
      int val;
    } leaf;

    struct constree* braces;

    struct {
      struct constree* lm;
      struct constree* rm;
    } node;
  }
} constree_s;

constree_s* new_leaf(int stat, int sign, int val);

constree_s* new_braces(constree_s** ct);

constree_s* new_node(int t, constree_s** lm, constree_s** rm);

constree_s* constree_from_str(const char* str);

int symbol_to_stat(char symbol[3]);

int eval(constree_s* ct);

constree_s* cutting(constree_s* ct);

void print_constree(constree_s* ct);

void free_constree(constree_s** ct);

/*

int symbol_to_stat(char symbol[3]);

void cleantree(constree_s** ct);

void printree(constree_s* ct);
*/



#endif
