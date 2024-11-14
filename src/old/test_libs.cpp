#include <stdio.h>
#include <stdlib.h>
#include <glpk.h>
#include <sqlite3.h>

int main(){
	printf("%s\n", glp_version());
	printf("%s\n", sqlite3_libversion());
	return EXIT_SUCCESS;
}
