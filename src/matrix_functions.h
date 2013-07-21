#ifndef matrix_functions_H 
#define matrix_functions_H

typedef struct {
  int ncols;
  int nrows;

  double *matrix; // Hoping to avoid the overhead in double**;
} matrix_t;

#define get_matrix_value(M, COL, ROW) ((M)->matrix[(COL) * (M)->nrows + (ROW)])
#define get_matrix_value(M, INDEX) ((M)->matrix[INDEX]) //  MUCH fast in many cases.

#define set_matrix_value(M, COL, ROW, VALUE) (M)->matrix[(COL) * (M)->nrows + (ROW)] = VALUE

matrix_t *alloc_matrix(int ncols, int nrows);
matrix_t *R_alloc_matrix(int ncols, int nrows);
void init_matrix(matrix_t *m, double init_value);
void init_matrix_rnorm(matrix_t *m, double mean, double varience);
void free_matrix(matrix_t *m);

void init_vector(double *vector, int n, double init_value);
double *vector_copy(double *vector, int n);

#define matrix_sum(M1, M2) for (int i = 0; i < ((M1)->ncols * (M1)->nrows); ++i) (M1)->matrix[i] += (M2)->matrix[i]
#define vector_sum(V1, V2, N) for (int i = 0; i < N; ++i) (V1)[i] += (V2)[i]

#endif
