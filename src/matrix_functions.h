#ifndef matrix_functions_H 
#define matrix_functions_H

typedef struct {
  int ncols;
  int nrows;

  double *matrix; // Hoping to avoid the overhead in double**;
} matrix_t;

matrix_t *alloc_matrix(int ncols, int nrows);
void init_matrix(matrix_t *m, double init_value);
void init_matrix_rnorm(matrix_t *m, double mean, double varience);
void free_matrix(matrix_t *m);
double get_matrix_value(matrix_t *m, int col, int row);
void set_matrix_value(matrix_t *m, int col, int row, double value);

void hadamard_product(matrix_t *m1, matrix_t *m2); // m1*m2; result in m1.
void matrix_sum(matrix_t *m1, matrix_t *m2); // m1+m2; result in m1.
//void matrix_dif(matrix_t *m1, matrix_t *m2); // m1-m2; result in m1.
//void times_scalar(matrix_t *m1, double a); // a*m1; result in m1.

void init_vector(double *vector, int n, double init_value);
void vector_product(double *vector1, double *vector2, int n);
void vector_sum(double *vector1, double *vector2, int n);
double *vector_difference_cpy(double *vector1, double *vector2, int n);

#endif

