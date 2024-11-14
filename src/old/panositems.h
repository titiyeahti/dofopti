#ifndef PANOSITEMS_H
#define PANOSITEMS_H

#include <data_import.h>

#define MAX_LENGTH 64

typedef struct pano{
  int id;
  char name[MAX_LENGTH];

  int max_size;
  //id of the first bonuses in the matrix
  int column_id;
  
  int nb_items;
  int items[nb_items];
} pano_t;

typedef struct item{
  int id;
  char name[MAX_LENGTH];
  int slot_code;
  int pano_id;
} item_t;

#endif
