#include "constree.h"

int main(){
  char * test = "CI>99&CA>99&CC>99&CS>99";

  constree_s* res = constree_from_str(test);
  printree(res);
  free_constree(res);

  char* test2 = "(CS>89&CI>89&CA>89)&(CP<12|CM<6)";

  res = constree_from_str(test2);
  printree(res);
  free_constree(res);

  char* test3 = "(Sc=968&SG=08&Sd>18&Sd<27)|PX=A";

  res = constree_from_str(test3);
  printree(res);
  /*cleantree(res);*/
  printree(res);
  free_constree(res);

  return 0;
}
