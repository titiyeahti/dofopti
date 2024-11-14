#include <stdio.h>
#include <stdlib.h>

typedef int stat_vect[52];

typedef struct {
    stat_vect sv;
    char name[64];
    int id;
} a_s;

typedef struct {
    stat_vect sv;
    int id;
    int bjr;
    char name[32];
} b_s;
typedef struct {
    char name[32];
    stat_vect sv;
    int id;
} c_s;
typedef struct {
    char name[32];
    int id;
    stat_vect sv;
} d_s;
typedef struct {
    int id;
    stat_vect sv;
    char name[32];
} f_s;
typedef struct {
    int id;
    char name[32];
    stat_vect sv;
} e_s;

int main(void){
    printf("%d\n", sizeof(stat_vect));
    printf("%d\n", sizeof(int));
    printf("%d\n", sizeof(a_s));
    printf("%d\n", sizeof(b_s));
    printf("%d\n", sizeof(c_s));
    printf("%d\n", sizeof(d_s));
    printf("%d\n", sizeof(e_s));
    printf("%d\n", sizeof(f_s));

    return 0;
}
