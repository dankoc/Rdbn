/************************************************************************************
 *  Functions for training. 
 * 
 *  Many of these are for manipulating matrices.  Try using naive functions ...
 *  but if taining is slow going, this is something to optimize.
 */
 
#include "matrix_functions.h"
#include "rbm.h"
 
/*****************************************************************************************************
 * Some matrix functions required for the weight matrix.
 */
 
double **alloc_matrix(int ncols, int nrows) {
  double **prod_mat= (double**)calloc(ncols, sizeof(double*));
  for(int i=0;i<ncols;i++)
    prod_mat[i]= (double*)calloc(nrows, sizeof(double*));
  return(prod_mat);
}

void init_matrix(double **matrix, int ncols, int nrows, double init_value) {
  for(int i=0;i<ncols;i++)
    for(int j=0;j<nrows;j++)
	  matrix[i][j]= init_value;
}

void free_matrix(double **matrix, int ncols) {
  for(int i=0;i<ncols;i++)
    free(matrix[i]);
  free(matrix);
}

/*
 * Element-by-element product of two vectors, returned in vector1.
 */
void vector_product(double *vector1, double *vector2, int n) {
  for(int i=0;i<n;i++)
    vector1[i] *= vector2[i];
}

/*
 * Computes the Hadamard product of two matrices.
 *
 * Multiplies matrix1 and matrix2.  The result will be in matrix1.
 *
 * Probably a more efficient way to compute this...
 */
void hadamard_product(double **matrix1, double **matrix2, int n_cols, int n_rows) {
  for(int i=0;i<n_cols;i++)
    for(int j=0;j<n_cols;j++)
	  matrix1[i][j] *= matrix2[i][j];
}

/*
 * Compute a matrix representing all posible products of two vectors, output and input.
 *
 */
void init_freq_matrix(rbm_t *rbm, double *output, double *input, double **delta_weights) {
  // Compute the frequency with which output_i and input_j occur together.
  for(int i=0;i<rbm[0].n_outputs;i++)
    for(int j=0;j<rbm[0].n_inputs;j++)
      delta_weights[i][j]= output[i]*input[j];
}

/*
 * Subtract each element of recon from data.  The result will be passed back in data.
 */
void compute_delta_w(rbm_t *rbm, double **data, double *output_recon, double *input_recon) {
  for(int i=0;i<rbm[0].n_outputs;i++)
    for(int j=0;j<rbm[0].n_inputs;j++)
      data[i][j]-= output_recon[i]*input_recon[j];
}

/*
 * Add matricies io_weights and delta_w.  The result will be in io_weights.
 */
void apply_delta_w(rbm_t *rbm, double **delta_w, int batch_size) {
  for(int i=0;i<rbm[0].n_outputs;i++)
    for(int j=0;j<rbm[0].n_inputs;j++)
      rbm.io_weights[i][j]+= rbm[0].learning_rate*delta_w[i][j]/(double)batch_size;
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
double *vector_difference_cpy(double *vector1, double *vector2, int n) {
  double *difference= (double*)calloc(n, sizeof(double));
  for(int i=0;i<n;i++)
    difference[i] = vector1[i]-vector2[i];
  return(difference);
}

/*
 * Update output biases.
 */
void apply_delta_bias_output(rbm_t *rbm, double *delta_bias_output, int batch_size) {
  for(int i=0;i<rbm[0].n_outputs;i++) 
    rbm[0].bias_outputs[i] += rbm[0].learning_rate*delta_bias_output[i]/(double)batch_size;
}

/*
 * Update input biases.
 */
void apply_delta_bias_input(rbm_t *rbm, double *delta_bias_input, int batch_size) {
  for(int j=0;j<rbm[0].n_inputs;j++)
    rbm[0].bias_inputs[j] += rbm[0].learning_rate*delta_bias_input[j]/(double)batch_size;
}
