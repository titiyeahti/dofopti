#include "constree.h"

int main(){
  FILE* stream = fopen("inputfiles/allconsts.in", "r");
  char buffer[256];
  int c = 0;
  while (fgets(buffer, 255, stream)){
    c ++;
    buffer[strlen(buffer) -1 ] = '\0';
    printf("%d - %s\n", c, buffer);
    
    /*
    constree_s* ct;
    constree_s* c2c;
    ct = constree_from_str(buffer);
    c2c = cutting(ct);

    print_constree(ct);
    print_constree(c2c);

    free_constree(&ct);
    free_constree(&c2c);
    */
  }
  fclose(stream);
  return 0;
}
