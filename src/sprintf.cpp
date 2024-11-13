#include <stdio.h>
#include <string.h>
char nullbuffer[333] = {'\0'};
char buffer[333];
const char* str = 
"create table if not exists work_%s as select * from items as \
base_items where (%s) order by slot_code;\n";

int main(void){
  sprintf(buffer, str, "toto", "where michel");
  printf(buffer); 
  memcpy(buffer, nullbuffer, 333);
  printf(buffer); 
  return 0;
}
