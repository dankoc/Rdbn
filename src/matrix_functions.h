#ifndef matrix_functions_H 
#define matrix_functions_H

#include "rbm.h"

double **alloc_matrix(int ncols, int nrows);
void init_matrix(double **matrix, int ncols, int nrows, double init_value);
void free_matrix(double **matrix, int ncols);
void hadamard_product(double **matrix1, double **matrix2, int n_cols, int n_rows);
void init_freq_matrix(rbm_t *rbm, double *output, double *input, double **delta_weights);
void compute_delta_w(rbm_t *rbm, double **data, double *output_recon, double *input_recon);
void apply_delta_w(rbm_t *rbm, double **delta_w, int batch_size);

void init_vector(double *vector, int n, double init_value);
void vector_product(double *vector1, double *vector2, int n);
double *vector_difference_cpy(double *vector1, double *vector2, int n);
void apply_delta_bias_output(rbm_t *rbm, double *delta_bias_output, int batch_size);
void apply_delta_bias_input(rbm_t *rbm, double *delta_bias_input, int batch_size);

#endif

