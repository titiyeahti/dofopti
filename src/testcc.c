#include "composite_constraints.h"

int main(){
  char* s1 = "1+2";
  char* s2 = "2*pui+cha+doe+10";
  char* s3 = ".8*(pui-dof*3.3+(agi+int)*3.2+5)+rpt";

  double coeffs1[STATS_COUNT] = {0.};
  double coeffs2[STATS_COUNT] = {0.};
  double coeffs3[STATS_COUNT] = {0.};
  double 
    ct1=0, 
    ct2=0, 
    ct3=0;
  

  /*compconst_s* c1 = compconst_from_str(s1);
  compconst_s* c2 = compconst_from_str(s2);*/
  compconst_s* c3 = compconst_from_str(s3);
  
  puts(s3);
  print_cc(c3, 0);
  compconst_eval(c3, coeffs3, &ct3);
  /*
  compconst_eval(c2, coeffs2, &ct2);
  compconst_eval(c1, coeffs1, &ct1);

  free_compconst(&c1);
  free_compconst(&c2);
  */
  free_compconst(&c3);


  return EXIT_SUCCESS;
}
