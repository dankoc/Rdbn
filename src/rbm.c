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
#include "bigwiglib.h"
#include "read_genomic_data.h"
#include "rbm.h"
#include "matrix_functions.h"

/*************************************************************************************
 *  Functions of initiatlizing, allocating, and free-ing rbm_t.
 */

/*
 *  Allocates memory for an RBM.
 */
rbm_t *alloc_rbm(int n_inputs, int n_outputs) {
  rbm_t *rbm =  (rbm_t*)malloc(sizeof(rbm_t));
  
  rbm[0].n_inputs= n_inputs;
  rbm[0].n_outputs= n_outputs;
  rbm[0].bias_inputs= (double*)calloc(n_inputs, sizeof(double));
  rbm[0].bias_outputs= (double*)calloc(n_outputs, sizeof(double));
  
  rbm[0].io_weights= (double**)calloc(n_outputs, sizeof(double*));
  for(int i=0;i<n_outputs;i++)
    rbm[0].io_weights[i] = (double*)calloc(n_inputs, sizeof(double));

	return(rbm);
}

/*
 *  Frees an allocated rbm[0].
 */
void free_rbm(rbm_t *rbm) {
  for(int i=0;i<rbm[0].n_outputs;i++)
    free(rbm[0].io_weights[i]);
  free(rbm[0].io_weights);
  free(rbm[0].bias_inputs);
  free(rbm[0].bias_outputs);
  free(rbm);
}

/*
 *  Initalizes an RBM to random values.
 *
 *  Init. values follows settings from: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
 *
 *  Arguments: 
 *    rbm                   --> Restricted boltzman machine object.
 *    learning_rate         --> Learning rate of the rbm[0].
 *    expected_frequency_on --> If possible, set to log[pi=(1pi)] where pi is the proportion of training vectors in which unit i is on.
 *
 */
void init_rbm(rbm_t *rbm, double learning_rate, double expected_frequency_on) {
  rbm[0].learning_rate= learning_rate;

  for(int i=0;i<rbm[0].n_outputs;i++) {
    for(int j=0;j<rbm[0].n_inputs;j++) {
	  rbm[0].io_weights[i][j]= rnorm(0.0d, 0.01d);
	  if(i==0) // Only do this once.
	    rbm[0].bias_inputs[j]= expected_frequency_on; // log[pi=(1pi)] where pi is the proportion of training vectors in which unit i is on.
    }
    rbm[0].bias_outputs[i]= 0.0d;
  }
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
  double *states = (double*)calloc(n_states, sizeof(double));
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
 double get_input_prob(rbm_t *rbm, int input_index, double *output) {
  double prob= rbm[0].bias_inputs[input_index];
  for(int i=0;i<rbm[0].n_outputs;i++)
    prob+= output[i]*rbm[0].io_weights[i][input_index];
  return(logistic_function(prob));
}
 
/*
 *  Fixes the output nodes to the specified states, and samples the input.
 *
 *  Arguments: 
 *    rbm    --> Information on the boltzman machine.
 *    output --> The value of all output nodes ... Assumed to be a probability (i.e. [0,1], inclusive).
 */
void clamp_output(rbm_t *rbm, double *output, double *resulting_input) {
  for(int i=0;i<rbm[0].n_inputs;i++) // Get prob. of input node by summing over output states.
	resulting_input[i]= get_input_prob(rbm, i, output);
}

// void clamp_output_sample(rbm_t *rbm, double *output, double *resulting_input) {
  // clamp_output(rbm, output, resulting_input);
  // sample_states(resulting_input, rbm[0].n_inputs);
// }

/************************************************************************************
 *
 *  Functions to fix the output nodes to and sample values at the input nodes
 *
 ************************************************************************************/
 double get_output_prob(rbm_t *rbm, int output_index, double *input) {
  double prob= rbm[0].bias_outputs[output_index];
  for(int i=0;i<rbm[0].n_inputs;i++)
    prob+= input[i]*rbm[0].io_weights[output_index][i];
  return(logistic_function(prob));
}
 
/*
 *  Fixes the input nodes to the specified states, and samples the output.
 *
 *  Arguments: 
 *    rbm    --> Information on the boltzman machine.
 *    input  --> The value of all input nodes ... Assumed to be a probability (i.e. [0,1], inclusive).
 */
void clamp_input(rbm_t *rbm, double *input, double *resulting_output) {
  for(int i=0;i<rbm[0].n_outputs;i++) // Get prob. of input node by summing over output states.
	resulting_output[i]= get_output_prob(rbm, i, input);
}

// void clamp_input_sample(rbm_t *rbm, double *input, double *resulting_output) {
  // clamp_input(rbm, input, resulting_output); 
  // sample_states(resulting_output, rbm[0].n_outputs);
// }

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
 */
void train(rbm_t *rbm, double **input_example, int batch_size, int CDn) {
  // Thesse are for updating edge weights.
  double **data= alloc_matrix(rbm[0].n_outputs, rbm[0].n_inputs); // Will be (<v_i h_j>_{data} - <v_i h_j>_{recon}) representing one element of the batch.
  double **batch= alloc_matrix(rbm[0].n_outputs, rbm[0].n_inputs);// \prod_{batch} (<v_i h_j>_{data} - <v_i h_j>_{recon})
  init_matrix(batch, rbm[0].n_outputs, rbm[0].n_inputs, 1.0f); // Init. to 1; later multiply each **data matrix.
  
  double *output_recon= (double*)calloc(rbm[0].n_outputs, sizeof(double));
  double *input_recon= (double*)calloc(rbm[0].n_inputs, sizeof(double));

  // For updating biases (we'd call these priors).
  double *output_bias_batch= (double*)calloc(rbm[0].n_outputs, sizeof(double));
  double *input_bias_batch= (double*)calloc(rbm[0].n_inputs, sizeof(double));
  init_vector(output_bias_batch, rbm[0].n_outputs, 1);
  init_vector(input_bias_batch, rbm[0].n_inputs, 1);
  
  for(int i=0;i<batch_size;i++) { // Foreach item in the batch.
    clamp_input(rbm, input_example[i], output_recon); // Compute p(hj=1 | v)= logistic_sigmoid(b_j+\sum(v_i * w_ij))

    // Compute <vihj>_data ... in this computation, sample random states (?!).
    double* output_states= sample_states_cpy(output_recon, rbm[0].n_outputs);
    init_freq_matrix(rbm, output_states, input_example[i], data);
    
    // Run Gibbs sampling for CDn steps.
    for(int cd=0;cd<CDn;cd++) {
      clamp_output(rbm, output_recon, input_recon); // Get the input_recon(struction), using the output from the previous step.
      clamp_input(rbm, input_recon, output_recon); // Get the output_recon(struction), using the input from the previous step.
    }
     
    // Compute <vihj>_recon
    compute_delta_w(rbm, data, output_recon, input_recon);
    hadamard_product(batch, data, rbm[0].n_outputs, rbm[0].n_inputs);

    // Compute \delta_bias (a.k.a prior).
	double *delta_bias_output= vector_difference_cpy(output_states, output_recon); // CGD: SHOULD i BE USING SAMPLED STATES HERE?? DON'T THINK SO!!
	double *delta_bias_input= vector_difference_cpy(input_example[i], input_recon);
    vector_product(output_bias_batch, delta_bias_output, rbm[0].n_outputs);
	vector_product(input_bias_batch, delta_bias_input, rbm[0].n_inputs);
	
    free(output_states);
	free(delta_bias_output);
	free(delta_bias_input);
  }
  // Update weights. \delta w_{ij} = \epislon * (<v_i h_j>_data - <v_i h_j>recon)
  apply_delta_w(rbm, batch, batch_size);
  apply_delta_bias_output(rbm, delta_bias_output, batch_size);
  apply_delta_bias_input(rbm, delta_bias_input, batch_size);
  
  // Cleanup temporary variables ...  
  free(output_recon);
  free(input_recon);
  free_matrix(data, rbm[0].n_outputs);
  free_matrix(batch, rbm[0].n_outputs);
  free(output_bias_batch);
  free(input_bias_batch);
}
