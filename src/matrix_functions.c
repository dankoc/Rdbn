/*****************************************************************************************************
 * Naive implementation of matrix functions.
 *
 * Try to allow vectors to be either 1xN or Nx1 matrices.
 */
 
#include "matrix_functions.h"
 
matrix_t *alloc_matrix(int ncols, int nrows) {
  matrix_t *m;
  m[0].matrix= (double*)calloc(ncols*nrows, sizeof(double));
  m[0].ncols= ncols;
  m[0].nrows= nrows;
  return(m);
}

void init_matrix(matrix_t *m, double init_value) {
  for(int i=0;i<(m[0].ncols*m[0].nrows);i++)
	  m[0].matrix[i]= init_value;
}

void init_matrix_rnorm(matrix_t *m, double mean, double varience) {
  for(int i=0;i<(m[0].ncols*m[0].nrows);i++)
	  matrix[0].m[i]= rnorm(mean, varience);
}

void free_matrix(matrix_t *m) {
  free(m[0].matrix);
  free(m); // (??)
}

double get_matrix_value(matrix_t *m, int col, int row) {
  return(m[0].matrix[col*m[0].nrows+row]); // Is this what R assumes?!
}

void set_matrix_value(matrix_t *m, int col, int row, double value) {
  m[0].matrix[col*m[0].nrows+row]= value; // Is this what R assumes?!
}


/*
 * Computes the Hadamard product of two matrices.
 *
 * Multiplies matrices m1 and m2.  The result will be in m1.
 *
 * Probably a more efficient way to compute this...
 */
void hadamard_product(matrix_t *m1, matrix_t *m2) {
  for(int i=0;i<(m1[0].ncols*m1[0].nrows);i++)
	  m1[0].matrix[i] *= m2[0].matrix[i];
}

/*
 * Computes the sum of two matrices, m1 and m2.  The result will be in m1.
 */
void matrix_sum(matrix_t m1, matrix_t m2) {
  for(int i=0;i<(m1[0].ncols*m1[0].nrows);i++)
	  m1[0].matrix[i] += m2[0].matrix[i];
}

/*****************************************************************************************************
 * Some vector functions required for biases.
 */
 
void init_vector(double *vector, int n, double init_value) {
  for(i=0;i<n;i++)
    vector[i]= init_value;
}

/*
 * Element-by-element product of two vectors, returned in vector1.
 */
void vector_product(double *vector1, double *vector2, int n) {
  for(int i=0;i<n;i++)
    vector1[i] *= vector2[i];
}

/*
 * Element-by-element sum of two vectors, returned in vector1.
 */
void vector_sum(double *vector1, double *vector2, int n) {
  for(int i=0;i<n;i++)
    vector1[i] += vector2[i];
}


/*
 * Element-by-element product of two vectors, returned in vector1.
 */
double *vector_difference_cpy(double *vector1, double *vector2, int n) {
  double *difference= (double*)calloc(n, sizeof(double));
  for(int i=0;i<n;i++)
    difference[i] = vector1[i]-vector2[i];
  return(difference);
}
