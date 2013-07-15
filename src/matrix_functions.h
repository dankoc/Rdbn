#ifndef matrix_functions_H 
#define matrix_functions_H

typedef struct {
  int ncols;
  int nrows;

  double *matrix; // Hoping to avoid the overhead in double**;
} matrix_t;

#define get_matrix_value(m, col, row) (m[0].matrix[col*m[0].nrows+row])
#define set_matrix_value(m, col, row, value) (m[0].matrix[col*m[0].nrows+row]= value)

matrix_t *alloc_matrix(int ncols, int nrows);
matrix_t *R_alloc_matrix(int ncols, int nrows);
void init_matrix(matrix_t *m, double init_value);
void init_matrix_rnorm(matrix_t *m, double mean, double varience);
void free_matrix(matrix_t *m);

void init_vector(double *vector, int n, double init_value);
double *vector_copy(double *vector, int n);

#define matrix_sum(m1, m2) (for(int i=0;i<(m1[0].ncols*m1[0].nrows);i++) m1[0].matrix[i] += m2[0].matrix[i]) /* (matrix_t*, matrix_t*) */
#define vector_sum(vector1, vector2, n) (for(int i=0;i<n;i++) vector1[i] += vector2[i]) /* (double* double* int) */

#endif

