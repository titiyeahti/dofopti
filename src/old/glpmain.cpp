#include <glpk.h>
#include <stdio.h>


// creation d'un probleme factice
#define N 100
#define M 10

void print_vec(size_t n, double vec[]){
  int j;
  for(j=0; j<n; j++){
    printf("%f\t", vec[j]);
  }
  printf("\n");
}

void print_matrix(size_t n, size_t m, double matrix[]){
  int i, j;
  for(i=0; i<n; i++){
    for(j=0; j<m; j++){
      printf("%f\t", matrix[i*m+j]);
    }
    printf("\n");
  }
}

int stat_to_basis(size_t n, size_t m, double matrix[], 
    double input[], double output[]){
  int i, j;

  double cur;

  for(i=0; i<n; i++){
    cur = 0;
    for(j=0; j<m; j++){
      cur += input[j]*matrix[i*m+j];
    }
    output[i] = cur;
  }

  return 0;
}

int basis_to_stat(size_t n, size_t m, double matrix[],
    double input[], double output[]){
  int i, j;

  double cur;

  for(i=0; i<m; i++){
    cur = 0;
    for(j=0; j<n; j++){
      cur += input[j]*matrix[j*m+i];
    }
    output[i] = cur;
  }

  return 0;
}

int main(void){
  size_t n = 22;
  size_t m = 3;
  double matrix[] = {
    0., 100., 500.,
    1., 70., 350.,
    0., 70, 300,
    0, 50, 350, 
    0, 80, 350,
    0, 0, 250,
    0, 100, 400,
    0, 100, 400,
    1, 80, 350,
    0, 90, 300,
    0, 80, 400,
    0, 0, 0,
    0, 0, 0,
    0, 100, 0,
    0, 0, 0,
    0, 0, 0,
    0, 40, 0,
    1, 40, 0,
    0, 0, 0,
    0, 0, 0,
    0, 50, 300,
    1, 50, 300
  };

  int i, j;
  glp_prob * pb;
  pb = glp_create_prob();
  //basis info
  glp_set_prob_name(pb, "miniprob");
  glp_set_obj_name(pb, "maxfo");
  glp_set_obj_dir(pb, GLP_MAX);
  glp_add_cols(pb, n);
  //add each item
  glp_set_col_name(pb, 1, "CH_H");
  glp_set_col_name(pb, 2, "KT_H");
  glp_set_col_name(pb, 3, "BB_R");
  glp_set_col_name(pb, 4, "CH_R");
  glp_set_col_name(pb, 5, "CR_R");
  glp_set_col_name(pb, 6, "VK_R");
  glp_set_col_name(pb, 7, "BB_T");
  glp_set_col_name(pb, 8, "VK_T");
  glp_set_col_name(pb, 9, "GE_T");
  glp_set_col_name(pb, 10, "BB_B");
  glp_set_col_name(pb, 11, "CH_B");
  glp_set_col_name(pb, 12, "VK_0");
  glp_set_col_name(pb, 13, "VK_1");
  glp_set_col_name(pb, 14, "VK_2");
  glp_set_col_name(pb, 15, "BB_0");
  glp_set_col_name(pb, 16, "BB_1");
  glp_set_col_name(pb, 17, "BB_2");
  glp_set_col_name(pb, 18, "BB_3");
  glp_set_col_name(pb, 19, "CH_0");
  glp_set_col_name(pb, 20, "CH_1");
  glp_set_col_name(pb, 21, "CH_2");
  glp_set_col_name(pb, 22, "CH_3");


  // boolean variables
  for(i=0; i<n; i++){
    glp_set_col_kind(pb, i+1, GLP_BV);
  }

  // crucru
  glp_set_col_kind(pb, 5, GLP_IV);
  glp_set_col_bnds(pb, 5, GLP_DB, 0., 2.);
  
  //add constraints integrity os slots + min fo)
  glp_add_rows(pb, 5);

  // config constraints
  int ids[4+1];
  double vv[4+1] = {1., 1., 1., 1., 1.};

  ids[1] = 1;
  ids[2] = 2;
  glp_set_row_name(pb, 1, "head");
  glp_set_row_bnds(pb, 1, GLP_UP, 0, 1);
  glp_set_mat_row(pb, 1, 2, ids, vv);

  ids[1] = 3;
  ids[2] = 4;
  ids[3] = 5;
  ids[4] = 6;
  glp_set_row_name(pb, 2, "ring");
  glp_set_row_bnds(pb, 2, GLP_UP, 0, 2);
  glp_set_mat_row(pb, 2, 4, ids, vv);

  ids[1] = 7;
  ids[2] = 8;
  ids[3] = 9;
  glp_set_row_name(pb, 3, "belt");
  glp_set_row_bnds(pb, 3, GLP_UP, 0, 1);
  glp_set_mat_row(pb, 3, 3, ids, vv);

  ids[1] = 10;
  ids[2] = 11;
  glp_set_row_name(pb, 4, "boots");
  glp_set_row_bnds(pb, 4, GLP_UP, 0, 1);
  glp_set_mat_row(pb, 4, 2, ids, vv);


  int paid[4+1] = {0, 2, 9, 18, 22};
  double paval[4+1] = {0, 1., 1., 1., 1.};
  glp_set_row_name(pb, 5, "MINPA");
  glp_set_row_bnds(pb, 5, GLP_LO, 2, 2);
  glp_set_mat_row(pb, 5, 4, paid, paval);


  double input[m] = {0, 1, 0};
  double output[n];
  stat_to_basis(n, m, matrix, input, output);

  print_vec(n, output);
  for(i=0; i<n; i++){
    glp_set_obj_coef(pb, i+1, output[i]);
  }

  // pano bonuses
  // coef 1 on items coef -i on pano bonuses variables
  // 2 const / pano
  glp_add_rows(pb, 6); 
  int pids[7+1];
  double pvals[7+1];
  //volk

  pids[1] = 6; pids[2] = 8; pids[3] = 12;
  pids[4] = 13; pids[5] = 14;

  pvals[1]=1;
  pvals[2]=1;
  pvals[3]=0;
  pvals[4]=-1;
  pvals[5]=-2;

  glp_set_row_name(pb, 6, "VK");
  glp_set_row_bnds(pb, 6, GLP_FX, 0, 0);
  glp_set_mat_row(pb, 6, 5, pids, pvals);

  pvals[3] = 1;

  glp_set_row_name(pb, 7, "VKb");
  glp_set_row_bnds(pb, 7, GLP_FX, 1, 1);
  glp_set_mat_row(pb, 7, 3, pids+2, pvals);
  

  //bb
  pids[1] = 3; pids[2] = 7; pids[3] = 10;
  pids[4] = 15; pids[5] = 16; pids[6] = 17; pids[7] = 18;

  pvals[1]=1;
  pvals[2]=1;
  pvals[3]=1;
  pvals[4]=0;
  pvals[5]=-1;
  pvals[6]=-2;
  pvals[7]=-3;

  glp_set_row_name(pb, 8, "BB");
  glp_set_row_bnds(pb, 8, GLP_FX, 0, 0);
  glp_set_mat_row(pb, 8, 7, pids, pvals);
  pvals[4] = 1;
  glp_set_row_name(pb, 9, "BBb");
  glp_set_row_bnds(pb, 9, GLP_FX, 1, 1);
  glp_set_mat_row(pb, 9, 4, pids+3, pvals);

  // CH
  pids[1] = 1; pids[2] = 4; pids[3] = 11;
  pids[4] = 19; pids[5] = 20; pids[6] = 21; pids[7] = 22;

  pvals[1]=1;
  pvals[2]=1;
  pvals[3]=1;
  pvals[4]=0;
  pvals[5]=-1;
  pvals[6]=-2;
  pvals[7]=-3;

  glp_set_row_name(pb, 10, "CH");
  glp_set_row_bnds(pb, 10, GLP_FX, 0, 0);
  glp_set_mat_row(pb, 10, 7, pids, pvals);
  pvals[4] = 1;
  glp_set_row_name(pb, 11, "CHb");
  glp_set_row_bnds(pb, 11, GLP_FX, 1, 1);
  glp_set_mat_row(pb, 11, 4, pids+3, pvals);

  // config parm
  glp_iocp parm;
  glp_init_iocp(&parm);
  parm.presolve = GLP_ON;

  // solve
  glp_intopt(pb, &parm);

  glp_write_prob(pb, 0, "problem_output.txt");
  glp_write_lp(pb, NULL, "output.txt");
  glp_print_mip(pb, "sol.txt");
  glp_delete_prob(pb);

  return 0;
}
