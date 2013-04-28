/*
 * rbm.c -- Fast code to learn and use restricted Boltzman machines in R/C.
 * 
 * Contains most of the common functions required to evaluate a trained RBM...  
 *
 * Logic for training using contrastive divergence found in rbm.train.c.
 */
#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Applic.h>
#include "rbm.h"
#include "matrix_functions.h"


extern inline double rbm_sample_state(double prob);
extern inline double logistic_function(double value);

/************************************************************************************
 *
 *  Functions to fix the output nodes to and sample values at the input nodes
 *
 ************************************************************************************/
extern inline void clamp_output(rbm_t *rbm, double *output, double *resulting_input);
extern inline void clamp_input(rbm_t *rbm, double *input, double *resulting_output);

/*************************************************************************************
 *  Functions of initiatlizing, allocating, and free-ing rbm_t.
 */

/*
 *  Allocates memory for an RBM.
 */
rbm_t *alloc_rbm(int n_inputs, int n_outputs) {
  rbm_t *rbm=  (rbm_t*)Calloc(1, rbm_t);
  
  rbm[0].n_inputs= n_inputs;
  rbm[0].n_outputs= n_outputs;
  rbm[0].bias_inputs= (double*)Calloc(n_inputs, double);
  rbm[0].bias_outputs= (double*)Calloc(n_outputs, double);

  rbm[0].io_weights= alloc_matrix(n_outputs, n_inputs);

  return(rbm);
}

/*
 *  Frees an allocated rbm.
 */
void free_rbm(rbm_t *rbm) {
  free_matrix(rbm[0].io_weights);
  Free(rbm[0].bias_inputs);
  Free(rbm[0].bias_outputs);
  if(rbm[0].use_momentum)
    free_matrix(rbm[0].momentum);

  Free(rbm);
}

/*
 *  Initalizes an RBM to random values.
 *
 *  Init. values follows settings from: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
 *
 *  Arguments: 
 *    rbm                   --> Restricted boltzman machine object.
 *    learning_rate         --> Learning rate of the rbm.
 *    batch_size            --> The number of input examples in a mini-batch during training.
 *    cd_n                  --> Number of Gibbs sampling steps used during contrastive divergence.
 *    expected_frequency_on --> If possible, set to log[pi=(1pi)] where pi is the proportion of training vectors in which unit i is on.
 *
 */
void init_rbm(rbm_t *rbm, double learning_rate, int batch_size, int cd_n, double expected_frequency_on) {
  rbm[0].learning_rate= learning_rate;
  rbm[0].batch_size= batch_size;
  rbm[0].cd_n= cd_n;
  
  init_matrix_rnorm(rbm[0].io_weights, 0.0, 0.01);
  
  for(int i=0;i<rbm[0].n_outputs;i++)
    rbm[0].bias_outputs[i]= 0.0;
	
  for(int j=0;j<rbm[0].n_inputs;j++)
    rbm[0].bias_inputs[j]= expected_frequency_on;
}

/*************************************************************************************
 *  Convenience functions for a delta_w_t object.
 */

delta_w_t *alloc_dwt_from_rbm(rbm_t *rbm) {
  delta_w_t *batch= (delta_w_t*)Calloc(1, delta_w_t);
  batch[0].delta_w= alloc_matrix(rbm[0].n_outputs, rbm[0].n_inputs);// \prod_{batch} (<v_i h_j>_{data} - <v_i h_j>_{recon})
  batch[0].delta_output_bias= (double*)Calloc(rbm[0].n_outputs, double); // output_bias_batch
  batch[0].delta_input_bias= (double*)Calloc(rbm[0].n_inputs, double); // input_bias_batch
  init_matrix(batch[0].delta_w, 0.0f); // Init. to 1; later multiply each **data matrix.
  init_vector(batch[0].delta_output_bias, rbm[0].n_outputs, 0);
  init_vector(batch[0].delta_input_bias, rbm[0].n_inputs, 0);
  batch[0].input_bias_allocated= 1;
  batch[0].batch_size= rbm[0].batch_size;  // Used for updating weights.
//  batch[0].learning_rate= rbm[0].learning_rate;
  
  return(batch);
}

void free_delta_w_ptr(delta_w_t *dw, int n) {
  for(int i=0;i<n;i++) {
    free_matrix(dw[i].delta_w);
    Free(dw[i].delta_output_bias);

    if(dw[i].input_bias_allocated)
      Free(dw[i].delta_input_bias);
  }
  Free(dw);
}


/* Takes the sum of two delta_w_t variables, batch and dw.  Sum returned in batch. */
void sum_delta_w(delta_w_t *batch, delta_w_t *dw) {
  matrix_sum(batch[0].delta_w, dw[0].delta_w);
  vector_sum(batch[0].delta_output_bias, dw[0].delta_output_bias, batch[0].delta_w[0].ncols);
  if(batch[0].input_bias_allocated && dw[0].input_bias_allocated)
    vector_sum(batch[0].delta_input_bias, dw[0].delta_input_bias, batch[0].delta_w[0].nrows);
}

/************************************************************************************
 *
 *  Converts rbm S4 classes (in R) into C structures ...
 */

rbm_t *common_rbm_r_type_to_c(SEXP rbm_r) {
  rbm_t *rbm= (rbm_t*)R_alloc(1, sizeof(rbm_t));

  rbm[0].n_inputs= INTEGER(GET_SLOT(rbm_r,Rf_install("n_inputs")))[0];
  rbm[0].n_outputs= INTEGER(GET_SLOT(rbm_r,Rf_install("n_outputs")))[0];

  rbm[0].bias_outputs= REAL(GET_SLOT(rbm_r, Rf_install("bias_outputs")));

  rbm[0].io_weights = (matrix_t*)R_alloc(1, sizeof(matrix_t));
  rbm[0].io_weights[0].matrix= REAL(GET_SLOT(rbm_r, Rf_install("io_weights")));
  rbm[0].io_weights[0].ncols= rbm[0].n_outputs;
  rbm[0].io_weights[0].nrows= rbm[0].n_inputs;

  rbm[0].learning_rate= REAL(GET_SLOT(rbm_r,Rf_install("learning_rate")))[0];
  rbm[0].batch_size= INTEGER(GET_SLOT(rbm_r, Rf_install("batch_size")))[0];
  rbm[0].cd_n= INTEGER(GET_SLOT(rbm_r, Rf_install("cd_n")))[0];

  rbm[0].use_momentum= INTEGER(GET_SLOT(rbm_r, Rf_install("use_momentum")))[0];
  
  if(rbm[0].use_momentum) {
    rbm[0].momentum_decay= REAL(GET_SLOT(rbm_r, Rf_install("momentum_decay")))[0];
    rbm[0].momentum= R_alloc_matrix(rbm[0].n_outputs, rbm[0].n_inputs);
    init_matrix(rbm[0].momentum, 0);
	
    rbm[0].output_momentum= (double*)R_alloc(rbm[0].n_outputs, sizeof(double));
    init_vector(rbm[0].output_momentum, rbm[0].n_outputs, 0);

    rbm[0].input_momentum= (double*)R_alloc(rbm[0].n_inputs, sizeof(double));
    init_vector(rbm[0].input_momentum, rbm[0].n_inputs, 0);
  }

  rbm[0].use_l2_penalty= INTEGER(GET_SLOT(rbm_r, Rf_install("use_l2_penalty")))[0];
  if(rbm[0].use_l2_penalty) {
    rbm[0].weight_cost= REAL(GET_SLOT(rbm_r, Rf_install("weight_cost")))[0];
  }

  return(rbm);
}

rbm_t *rbm_layer_r_to_c(SEXP rbm_r, double *points_to_bias_inputs) {
  rbm_t *rbm= common_rbm_r_type_to_c(rbm_r);
  rbm[0].bias_inputs= points_to_bias_inputs;
  rbm[0].update_input_bias= 0;
  return(rbm);
}

rbm_t *rbm_r_to_c(SEXP rbm_r) {
  rbm_t *rbm= common_rbm_r_type_to_c(rbm_r);
  rbm[0].bias_inputs= REAL(GET_SLOT(rbm_r,Rf_install("bias_inputs")));
  rbm[0].update_input_bias= 1;
  return(rbm);
}

