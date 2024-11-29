#ifndef GLPAPI_H
#define GLPAPI_H

#include<glpk.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sqlite3.h>
#include "data_types.h"

extern const char* const stats_names[];

extern const char* const slots_names[];

extern double matrix_pptac[];


int init();
int fill_pptac();

void print_statline(statline_s* sl);

/*TODO UPDATE PRINTS*/
void print_pids(pids_s* pi);

void print_pbdata(pbdata_s* pbd);

int new_pbdata(sqlite3* db, pbdata_s* res, stat_vect base_stats,
    int targeted_slots[SLOT_COUNT], int level);

void free_pbdata(pbdata_s* pbd);

void print_vec(size_t n, double vec[]);

void print_matrix(size_t n, size_t m, double matrix[]);

void add_vec(size_t n, double v1[], double v2[], double output[]);

void scalar_mult_vec(size_t n, double lambda, double input[], double output[]);

int mat_times_vec(size_t n, size_t m, double matrix[], 
    double input[], double output[]);

int basis_to_stat(size_t n, size_t m, double matrix[],
    double input[], double output[]);

int stat_to_basis(size_t n, size_t m, double matrix[],
    double input[], double output[]);

linprob_s* new_linprob(pbdata_s* pbd);

int add_pano_constraints(glp_prob* pb, pids_s pano);

int solve_linprob(linprob_s* lp);

int const_linear(linprob_s* lp, double coeffs[], const char* name, double* const_term);

int const_linear_upper(linprob_s* lp, double coeffs[],
    double bound, const char* name);

int const_linear_lower(linprob_s* lp, double coeffs[],
    double bound, const char* name);

int const_linear_fix(linprob_s* lp, double coeffs[],
    double bound, const char* name);

int const_multi_simple_constraints(linprob_s* lp, double bounds[], int sign[]);

int set_obj_coeff(linprob_s* lp, double coeffs[]);

void free_linprob(linprob_s* lp);

void print_linsol(linprob_s* lp, pbdata_s* pbd, FILE* stream);

int const_lock_out_item(linprob_s* lp, const char* name);

int const_lock_in_item(linprob_s* lp, const char* name);
#endif
