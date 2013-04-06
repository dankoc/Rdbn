/*
 * Functions required to work with logs.  
 *
 */
#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Applic.h>
#include <assert.h>
#include "rbm.h"
#include "matrix_functions.h"

/*************************************************************************************
 *  Functions of initiatlizing, allocating, and free-ing rbm_t.
 */

/*
 *  Allocates memory for an RBM.
 */
rbm_t alloc_rbm(int n_inputs, int n_outputs) {
  rbm_t rbm;// =  (rbm_t*)malloc(sizeof(rbm_t));
  
  rbm.n_inputs= n_inputs;
  rbm.n_outputs= n_outputs;
  rbm.bias_inputs= (double*)Calloc(n_inputs, double);
  rbm.bias_outputs= (double*)Calloc(n_outputs, double);

  rbm.io_weights= alloc_matrix(n_outputs, n_inputs);

  return(rbm);
}

/*
 *  Frees an allocated rbm.
 */
void free_rbm(rbm_t rbm) {
  free_matrix(rbm.io_weights);
  Free(rbm.bias_inputs);
  Free(rbm.bias_outputs);
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
rbm_t init_rbm(rbm_t rbm, double learning_rate, int batch_size, int cd_n, double expected_frequency_on) {
  rbm.learning_rate= learning_rate;
  rbm.batch_size= batch_size;
  rbm.cd_n= cd_n;
  
  init_matrix_rnorm(rbm.io_weights, 0.0d, 0.01d);
  
  for(int i=0;i<rbm.n_outputs;i++)
    rbm.bias_outputs[i]= 0.0d;
	
  for(int j=0;j<rbm.n_inputs;j++)
    rbm.bias_inputs[j]= expected_frequency_on;
  
  return(rbm);
}

/*************************************************************************************
 *  Common functions required for many of the input/ output clamping and fitting ...
 */

/*
 *  Changes prob to 0 || 1 by sampling over a uniform.
 */
void sample_states(double *prob, int n_states) {
  for(int i=0;i<n_states;i++)
    prob[i]= prob[i]>runif(0.0d, 1.0d)?1:0;
}

double *sample_states_cpy(double *prob, int n_states) {
  double *states = (double*)Calloc(n_states, double);
  for(int i=0;i<n_states;i++)
    states[i]= prob[i]>runif(0.0d, 1.0d)?1:0;
  return(states);
}

double logistic_function(double value) {
  return(1/ (1+ exp(-value)));
}

/************************************************************************************
 *
 *  Functions to fix the output nodes to and sample values at the input nodes
 *
 ************************************************************************************/
 double get_input_prob(rbm_t rbm, int input_index, double *output) {
  double prob= rbm.bias_inputs[input_index];
  for(int i=0;i<rbm.n_outputs;i++)
    prob+= output[i]*get_matrix_value(rbm.io_weights, i, input_index);
  return(logistic_function(prob));
}
 
/*
 *  Fixes the output nodes to the specified states, and samples the input.
 *
 *  Arguments: 
 *    rbm    --> Information on the boltzman machine.
 *    output --> The value of all output nodes ... Assumed to be a probability (i.e. [0,1], inclusive).
 */
void clamp_output(rbm_t rbm, double *output, double *resulting_input) {
  for(int i=0;i<rbm.n_inputs;i++) // Get prob. of input node by summing over output states.
	resulting_input[i]= get_input_prob(rbm, i, output);
}

/************************************************************************************
 *
 *  Functions to fix the output nodes to and sample values at the input nodes
 *
 ************************************************************************************/
 double get_output_prob(rbm_t rbm, int output_index, double *input) {
  double prob= rbm.bias_outputs[output_index];
  for(int i=0;i<rbm.n_inputs;i++)
    prob+= input[i]*get_matrix_value(rbm.io_weights, output_index, i);
  return(logistic_function(prob));
}
 
/*
 *  Fixes the input nodes to the specified states, and samples the output.
 *
 *  Arguments: 
 *    rbm    --> Information on the boltzman machine.
 *    input  --> The value of all input nodes ... Assumed to be a probability (i.e. [0,1], inclusive).
 */
void clamp_input(rbm_t rbm, double *input, double *resulting_output) {
  for(int i=0;i<rbm.n_outputs;i++) // Get prob. of input node by summing over output states.
	resulting_output[i]= get_output_prob(rbm, i, input);
}


/**********************************************************************
 Functions for getting/ updating during training. */
 
/*Compute a matrix representing all posible products of two vectors, output and input.*/
void init_freq_matrix(rbm_t rbm, double *output, double *input, matrix_t *delta_weights) {
  // Compute the frequency with which output_i and input_j occur together.
  for(int i=0;i<rbm.n_outputs;i++)
    for(int j=0;j<rbm.n_inputs;j++)
      set_matrix_value(delta_weights, i, j, output[i]*input[j]);
}

/*Subtract each element of recon from data.  The result will be passed back in data.*/
void compute_delta_w(rbm_t rbm, matrix_t *data, double *output_recon, double *input_recon) {
  for(int i=0;i<rbm.n_outputs;i++)
    for(int j=0;j<rbm.n_inputs;j++) {
      double data_i_j_old= get_matrix_value(data, i, j);
      double data_i_j_new= data_i_j_old-output_recon[i]*input_recon[j];
      set_matrix_value(data, i, j, data_i_j_new); // Really need to inline these setter-getter functions.
    }
}

/*Add matricies io_weights and delta_w.  The result will be in io_weights.*/
void apply_delta_w(rbm_t rbm, matrix_t *delta_w) {
  for(int i=0;i<rbm.n_outputs;i++)
    for(int j=0;j<rbm.n_inputs;j++) {
      double previous_w_i_j= get_matrix_value(rbm.io_weights, i, j);
      double delta_w_i_j= get_matrix_value(delta_w, i, j);
      double new_w_i_j= previous_w_i_j+rbm.learning_rate*delta_w_i_j/(double)rbm.batch_size;
      set_matrix_value(rbm.io_weights, i, j, new_w_i_j);
    }
}

/*Update output biases.*/
void apply_delta_bias_output(rbm_t rbm, double *delta_bias_output) {
  for(int i=0;i<rbm.n_outputs;i++) 
    rbm.bias_outputs[i] += rbm.learning_rate*delta_bias_output[i]/(double)rbm.batch_size;
}

/*Update input biases.*/
void apply_delta_bias_input(rbm_t rbm, double *delta_bias_input) {
  for(int j=0;j<rbm.n_inputs;j++)
    rbm.bias_inputs[j] += rbm.learning_rate*delta_bias_input[j]/(double)rbm.batch_size;
}

/*  
 * Processes a single batch element, and increments the delta_w, delta_bias_input, and delta_bias_output
 *  weights accordingly.
 *
 * Arguments:
 *    rbm --> Information on the restricted boltzman machine.
 *    
 *
 */
void do_batch_member(rbm_t rbm,  double *input_example, matrix_t *batch, double *output_bias_batch, double *input_bias_batch) {
  matrix_t *data= alloc_matrix(rbm.n_outputs, rbm.n_inputs); // Will be (<v_i h_j>_{data} - <v_i h_j>_{recon}) representing one element of the batch.
  double *output_recon= (double*)Calloc(rbm.n_outputs, double);
  double *input_recon= (double*)Calloc(rbm.n_inputs, double);

  clamp_input(rbm, input_example, output_recon); // Compute p(hj=1 | v)= logistic_sigmoid(b_j+\sum(v_i * w_ij))

  // Compute <vihj>_data ... in this computation, sample random states (?!).
  double* output_states= sample_states_cpy(output_recon, rbm.n_outputs);
  init_freq_matrix(rbm, output_states, input_example, data);
    
  // Run Gibbs sampling for CDn steps.
  for(int cd=0;cd<rbm.cd_n;cd++) {
    clamp_output(rbm, output_recon, input_recon); // Get the input_recon(struction), using the output from the previous step.
    clamp_input(rbm, input_recon, output_recon); // Get the output_recon(struction), using the input from the previous step.
  }
     
  // Compute <vihj>_recon
  compute_delta_w(rbm, data, output_recon, input_recon);
  matrix_sum(batch, data); // Update batch.

  // Compute \delta_bias (a.k.a prior).
  clamp_input(rbm, input_example, output_states); // CGD: SHOULD i BE USING SAMPLED STATES WHEN COMPUTING delta_bias_output?? i DON'T THINK SO!!
  double *delta_bias_output= vector_difference_cpy(output_states, output_recon, rbm.n_outputs); 
  double *delta_bias_input= vector_difference_cpy(input_example, input_recon, rbm.n_inputs);
  vector_sum(output_bias_batch, delta_bias_output, rbm.n_outputs); // Update output_bias_batch.
  vector_sum(input_bias_batch, delta_bias_input, rbm.n_inputs); // Update input_bias_batch

  Free(output_states);	Free(delta_bias_output);	Free(delta_bias_input);
  Free(output_recon);   Free(input_recon);
  free_matrix(data);  
}

/************************************************************************************
 *
 *  Optimize the weights weights over a training dataset using contrastive divergence.
 *
 *  Arguments:
 *    rbm           --> Restricted boltzman machine.
 *    input_example --> A matrix (n_inptus x batch_size) of input examples.  
 *    batch_size    --> Specifies the number of input examples provided.  
 *    CDn           --> Number of iterations of Gibbs sampling before taking the resulting state.
 *
 *  Following recommendations in: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf (V. 1; dated: Aug. 2nd 2010).
 *
 * Code currently trains by taking the MEAN of gradients in a batch!  This seems most intiutive to me, and more
 *  or less agrees with comments in: http://www.elen.ucl.ac.be/Proceedings/esann/esannpdf/es2012-71.pdf.
 * Don't know what all these how-to pages are talking about when they say 'multiplying matrices', though!?
 */
void do_minibatch(rbm_t rbm, double *input_example) { // Use velocity?!; Use sparsity target?!  // Change name to 
  // Thesse are for updating edge weights.
  matrix_t *batch= alloc_matrix(rbm.n_outputs, rbm.n_inputs);// \prod_{batch} (<v_i h_j>_{data} - <v_i h_j>_{recon})
  init_matrix(batch, 0.0f); // Init. to 1; later multiply each **data matrix.
  
  // For updating biases (we'd call these priors).
  double *output_bias_batch= (double*)Calloc(rbm.n_outputs, double);
  double *input_bias_batch= (double*)Calloc(rbm.n_inputs, double);
  init_vector(output_bias_batch, rbm.n_outputs, 0);
  init_vector(input_bias_batch, rbm.n_inputs, 0);
  
  // How to asses convergence?!  Do that here?!
  for(int i=0;i<rbm.batch_size;i++) { // Foreach item in the batch.
    do_batch_member(rbm, input_example, batch, output_bias_batch, input_bias_batch);
    input_example+= rbm.n_inputs; // Update the input_example pointer to the next input sample.
  }
  
  // Update weights. \delta w_{ij} = \epislon * (<v_i h_j>_data - <v_i h_j>recon)
  apply_delta_w(rbm, batch);
  apply_delta_bias_output(rbm, output_bias_batch);
  apply_delta_bias_input(rbm, input_bias_batch);
  
  // Cleanup temporary variables ...  
  free_matrix(batch);
  Free(output_bias_batch);  Free(input_bias_batch);
}

/*
 * Shortcut to loop over examples for a given number of epocs.
 *
 * Arguments: 
 *   rbm           --> Restricted boltzman machine.
 *   input_example --> Input example used for training.  Each columns represents a unique training case.
 *   n_examples    --> Specifies the number of examples provided.
 *   n_epocs       --> Specifies the number of times the training program loops over the examples.
 *
 * Assumptions: 
 *   --> n_examples is a multiple of rbm.batch_size ... additional examples are ignored.
 */
void rbm_train(rbm_t rbm, double *input_example, int n_examples, int n_epocs) {
  double *current_position;
  int n_training_iterations= floor(n_examples/rbm.batch_size); 
  
  for(int i=0;i<n_epocs;i++) {
    current_position= input_example; // Reset training pointer.
    for(int j=0;j<n_training_iterations;j++) {
      do_minibatch(rbm, current_position);  // Do a minibatch using the current position of the training pointer.
      current_position+= rbm.batch_size*rbm.n_inputs; // Increment the input_example pointer batch_size # of columns.
	}
  }

}

/************************************************************************************
 *
 *  An R interface for RBM training ...
 */

rbm_t rbm_r_to_c(SEXP rbm_r) {
  rbm_t rbm;//= (rbm_t*)R_alloc(1, sizeof(rbm_t));

  rbm.n_inputs= INTEGER(GET_SLOT(rbm_r,Rf_install("n_inputs")))[0];
  rbm.n_outputs= INTEGER(GET_SLOT(rbm_r,Rf_install("n_outputs")))[0];

  rbm.bias_inputs= REAL(GET_SLOT(rbm_r,Rf_install("bias_inputs")));
  rbm.bias_outputs= REAL(GET_SLOT(rbm_r, Rf_install("bias_outputs")));

  rbm.io_weights = (matrix_t*)R_alloc(1, sizeof(matrix_t));
  rbm.io_weights[0].matrix= REAL(GET_SLOT(rbm_r, Rf_install("io_weights")));
  rbm.io_weights[0].ncols= rbm.n_outputs;
  rbm.io_weights[0].nrows= rbm.n_inputs;

  rbm.learning_rate= REAL(GET_SLOT(rbm_r,Rf_install("learning_rate")))[0];
  rbm.batch_size= INTEGER(GET_SLOT(rbm_r, Rf_install("batch_size")))[0];
  rbm.cd_n= INTEGER(GET_SLOT(rbm_r, Rf_install("cd_n")))[0];

  rbm.use_momentum= INTEGER(GET_SLOT(rbm_r, Rf_install("use_momentum")))[0];
  
  if(rbm.use_momentum) {
    rbm.momentum_decay= REAL(GET_SLOT(rbm_r, Rf_install("momentum_decay")));
    rbm.momentum= (matrix_t*)R_alloc(1, sizeof(matrix_t));
	
    rbm.momentum[0].matrix= REAL(GET_SLOT(rbm_r, Rf_install("momentum")));
    rbm.momentum[0].ncols= rbm.n_outputs;
    rbm.momentum[0].nrows= rbm.n_inputs;
  }

  return(rbm);
}

 
SEXP train_rbm_R(SEXP rbm_r, SEXP training_data_r, SEXP n_epocs_r) {
  rbm_t rbm= rbm_r_to_c(rbm_r); // Get values from R function.

  int n_examples= Rf_nrows(training_data_r)/rbm.n_inputs;
  int n_epocs= INTEGER(n_epocs_r)[0];
  double *input_example= REAL(training_data_r);

  rbm_train(rbm, input_example, n_examples, n_epocs);
  
  return(rbm_r);
}


