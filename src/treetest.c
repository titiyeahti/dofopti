#include "constree.h"

int main(){

  constree_s* ct0;
  constree_s* ct1;
  constree_s* ct2;
  constree_s* ct3;
  constree_s* ct4;
  constree_s* ct5;

  /*
  ct0 = new_leaf(VITA, -1, 2500);
  ct1 = new_leaf(FOR, -1, 200);
  ct2 = new_node(AND, &ct0, &ct1);
  ct3 = new_braces(&ct2);
  ct4 = new_leaf(PA, -1, 12);
  ct5 = new_node(OR, &ct3, &ct4);
  print_constree(ct5);
  free_constree(&ct5);
  */

  char* str0 = "CS>399|CI>399|CA>399";
  char* str1 = "CS>449&(Qf=2187|(Qa=2187&Qo>16838))";
  char* str2 = "CS>299&(CI>299|CC>299|CA>299)";

  /*
  ct5 = constree_from_str(str0);
  print_constree(ct5);
  free_constree(&ct5);

  ct5 = constree_from_str(str1);
  print_constree(ct5);
  free_constree(&ct5);
*/
  ct5 = constree_from_str(str1);
  print_constree(ct5);
  ct1 = eval(ct5);
  print_constree(ct1);
  free_constree(&ct5);
  free_constree(&ct1);
  return 0;
}
