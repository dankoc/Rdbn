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

/************************************************************************************
 *
 *  Functions to fix the output nodes to and sample values at the input nodes
 *
 ************************************************************************************/
void clamp_input(rbm_t *rbm, double *input, double *resulting_output) {
  for(int i=0;i<rbm->n_outputs;i++) {// Get prob. of input node by summing over output states.
    resulting_output[i]= rbm->bias_outputs[i];
    for(int j=0;j<rbm->n_inputs;j++) {
      resulting_output[i]+= input[j]*get_matrix_value(rbm->io_weights, i, j);
    }
        resulting_output[i]= logistic_function(resulting_output[i]);
  }
}

void clamp_output(rbm_t *rbm, double *output, double *resulting_input)  {
  for(int i=0;i<rbm->n_inputs;i++) {// Get prob. of input node by summing over output states.
    resulting_input[i]= rbm->bias_inputs[i];
    for(int j=0;j<rbm->n_outputs;j++) {
      resulting_input[i]+= output[j]*get_matrix_value(rbm->io_weights, j, i);
        }
        resulting_input[i]= logistic_function(resulting_input[i]);
  }
}


/*************************************************************************************
 * Preforms alternating Gibbs sampling on *rbm.  Returns a reconstructed input example.
 *
 */
double *daydream(rbm_t *rbm, int cd_n, double *input_example) {
  double *output_recon= (double*)Calloc(rbm->n_outputs, double);
  clamp_input(rbm, input_example, output_recon); // Compute p(hj=1 | v)= logistic_sigmoid(b_j+\sum(v_i * w_ij))
  double *input_recon= (double*)Calloc(rbm->n_inputs, double);

  for(int cd=0;cd<cd_n;cd++) {
    sample_states(output_recon, rbm->n_outputs); // Sample the hidden states.
    clamp_output(rbm, output_recon, input_recon); // Get the input_recon(struction), using the output from the previous step.
    clamp_input(rbm, input_recon, output_recon); // Get the output_recon(struction), using the input from the previous step.
  }
  
  return(input_recon);
}

/*************************************************************************************
 *  Functions of initiatlizing, allocating, and free-ing rbm_t.
 */

/*
 *  Allocates memory for an RBM.
 */
rbm_t *alloc_rbm(int n_inputs, int n_outputs) {
  rbm_t *rbm=  (rbm_t*)Calloc(1, rbm_t);
  
  rbm->n_inputs= n_inputs;
  rbm->n_outputs= n_outputs;
  rbm->bias_inputs= (double*)Calloc(n_inputs, double);
  rbm->bias_outputs= (double*)Calloc(n_outputs, double);

  rbm->io_weights= alloc_matrix(n_outputs, n_inputs);

  return(rbm);
}

/*
 *  Frees an allocated rbm.
 */
void free_rbm(rbm_t *rbm) {
  free_matrix(rbm->io_weights);
  Free(rbm->bias_inputs);
  Free(rbm->bias_outputs);
  if(rbm->use_momentum)
    free_matrix(rbm->momentum);

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
  rbm->learning_rate= learning_rate;
  rbm->batch_size= batch_size;
  rbm->cd_n= cd_n;
  
  init_matrix_rnorm(rbm->io_weights, 0.0, 0.01);
  
  for(int i=0;i<rbm->n_outputs;i++)
    rbm->bias_outputs[i]= 0.0;

  for(int j=0;j<rbm->n_inputs;j++)
    rbm->bias_inputs[j]= expected_frequency_on;
}

/*************************************************************************************
 *  Convenience functions for a delta_w_t object.
 */

delta_w_t *alloc_dwt_from_rbm(rbm_t *rbm) {
  delta_w_t *batch= (delta_w_t*)Calloc(1, delta_w_t);
  batch->delta_w= alloc_matrix(rbm->n_outputs, rbm->n_inputs);// \prod_{batch} (<v_i h_j>_{data} - <v_i h_j>_{recon})
  batch->delta_output_bias= (double*)Calloc(rbm->n_outputs, double); // output_bias_batch
  batch->delta_input_bias= (double*)Calloc(rbm->n_inputs, double); // input_bias_batch
  init_matrix(batch->delta_w, 0.0f); // Init. to 1; later multiply each **data matrix.
  init_vector(batch->delta_output_bias, rbm->n_outputs, 0);
  init_vector(batch->delta_input_bias, rbm->n_inputs, 0);
  batch->input_bias_allocated= 1;
  batch->batch_size= rbm->batch_size;  // Used for updating weights.
//  batch->learning_rate= rbm->learning_rate;
  
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

/************************************************************************************
 *
 *  Converts rbm S4 classes (in R) into C structures ...
 */

rbm_t *common_rbm_r_type_to_c(SEXP rbm_r) {
  rbm_t *rbm= (rbm_t*)R_alloc(1, sizeof(rbm_t));

  rbm->n_inputs= INTEGER(GET_SLOT(rbm_r,Rf_install("n_inputs")))[0];
  rbm->n_outputs= INTEGER(GET_SLOT(rbm_r,Rf_install("n_outputs")))[0];

  rbm->bias_outputs= REAL(GET_SLOT(rbm_r, Rf_install("bias_outputs")));

  rbm->io_weights = (matrix_t*)R_alloc(1, sizeof(matrix_t));
  rbm->io_weights->matrix= REAL(GET_SLOT(rbm_r, Rf_install("io_weights")));
  rbm->io_weights->ncols= rbm->n_outputs;
  rbm->io_weights->nrows= rbm->n_inputs;

  rbm->learning_rate= REAL(GET_SLOT(rbm_r,Rf_install("learning_rate")))[0];
  rbm->batch_size= INTEGER(GET_SLOT(rbm_r, Rf_install("batch_size")))[0];
  rbm->cd_n= INTEGER(GET_SLOT(rbm_r, Rf_install("cd_n")))[0];

  rbm->use_momentum= INTEGER(GET_SLOT(rbm_r, Rf_install("use_momentum")))[0];
  
  if(rbm->use_momentum) {
    rbm->momentum_decay= REAL(GET_SLOT(rbm_r, Rf_install("momentum_decay")))[0];
    rbm->momentum= R_alloc_matrix(rbm->n_outputs, rbm->n_inputs);
    init_matrix(rbm->momentum, 0);

    rbm->output_momentum= (double*)R_alloc(rbm->n_outputs, sizeof(double));
    init_vector(rbm->output_momentum, rbm->n_outputs, 0);

    rbm->input_momentum= (double*)R_alloc(rbm->n_inputs, sizeof(double));
    init_vector(rbm->input_momentum, rbm->n_inputs, 0);
  }

  rbm->use_l2_penalty= INTEGER(GET_SLOT(rbm_r, Rf_install("use_l2_penalty")))[0];
  if(rbm->use_l2_penalty) {
    rbm->weight_cost= REAL(GET_SLOT(rbm_r, Rf_install("weight_cost")))[0];
  }

  return(rbm);
}

rbm_t *rbm_layer_r_to_c(SEXP rbm_r, double *points_to_bias_inputs) {
  rbm_t *rbm= common_rbm_r_type_to_c(rbm_r);
  rbm->bias_inputs= points_to_bias_inputs;
  rbm->update_input_bias= 0;
  return(rbm);
}

rbm_t *rbm_r_to_c(SEXP rbm_r) {
  rbm_t *rbm= common_rbm_r_type_to_c(rbm_r);
  rbm->bias_inputs= REAL(GET_SLOT(rbm_r,Rf_install("bias_inputs")));
  rbm->update_input_bias= 1;
  return(rbm);
}
