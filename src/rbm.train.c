/*
 * rbm.train.c -- Trains a restricted Boltzman machine using contrastive divergence.
 * 
 *
 */
#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Applic.h>
#include <omp.h>
#include "rbm.h"
#include "rbm.train.h"
#include "matrix_functions.h"

/**********************************************************************
 Functions for getting/ updating during training. */
 
/*
 * Add matricies io_weights and delta_w.  The result will be in io_weights.
 *
 * Also includes 
 */
void apply_delta_w(rbm_t *rbm, delta_w_t *dw) {
  #pragma omp parallel for
  for(int i=0;i<rbm[0].n_outputs;i++) {
    rbm[0].bias_outputs[i] += rbm[0].learning_rate*dw[0].delta_output_bias[i]/(double)dw[0].batch_size; 
    for(int j=0;j<rbm[0].n_inputs;j++) {
      double previous_w_i_j= get_matrix_value(rbm[0].io_weights, i, j);
      double delta_w_i_j= get_matrix_value(dw[0].delta_w, i, j);

      // If using L2 penalty (a.k.a "weight decay"), apply that here.
      if(rbm[0].use_l2_penalty) 
        delta_w_i_j-= rbm[0].weight_cost*previous_w_i_j; // Is this the right sign!?

      double new_w_i_j= previous_w_i_j+rbm[0].learning_rate*delta_w_i_j/(double)dw[0].batch_size;

      set_matrix_value(rbm[0].io_weights, i, j, new_w_i_j);

      if(i==0 && rbm[0].update_input_bias) // Only update once... and if everything says to update.
        rbm[0].bias_inputs[j] += rbm[0].learning_rate*dw[0].delta_input_bias[j]/(double)dw[0].batch_size;
    }
  }
}

/*
 * These functions for updating both weights and velocities together, using the momentum approach.
 *
 *  We use the momentum approach suggested by Nesterov, and adapted by Ilya Sutskever.
 *  Described here: http://www.cs.utoronto.ca/~ilya/pubs/ilya_sutskever_phd_thesis.pdf.
 *  See pp. 75 of the Sutskever thesis, and in particular equations 7.10-7.11.
 *
 */

void initial_momentum_step(rbm_t *rbm) {
  #pragma omp parallel for
  for(int i=0;i<rbm[0].n_outputs;i++) {
    rbm[0].output_momentum[i]*= rbm[0].momentum_decay;
    rbm[0].bias_outputs[i]+= rbm[0].output_momentum[i];
    for(int j=0;j<rbm[0].n_inputs;j++) {
      // Computes(eq 7.11, 1st half): v_t=\mu_{t-1}v_{t-1}.
      double momentum_i_j= rbm[0].momentum_decay*get_matrix_value(rbm[0].momentum, i, j); 
      set_matrix_value(rbm[0].momentum, i, j, momentum_i_j); // Updates momentum matrix.

      // Computes(eq 7.10, 1st half): \theta_t = \theta_{t-1} + \mu_{t-11}v_{t-1}.
      set_matrix_value(rbm[0].io_weights, i, j, get_matrix_value(rbm[0].io_weights, i, j)+momentum_i_j);

      if(i==0 && rbm[0].update_input_bias) {
        rbm[0].input_momentum[j]*= rbm[0].momentum_decay;
        rbm[0].bias_inputs[j]+= rbm[0].input_momentum[j];
      }
    }
  }
}
 
void apply_momentum_correction(rbm_t *rbm, delta_w_t *dw) {
  #pragma omp parallel for
  for(int i=0;i<rbm[0].n_outputs;i++) {
    rbm[0].bias_outputs[i]+= rbm[0].learning_rate*dw[0].delta_output_bias[i]/(double)dw[0].batch_size; 
    rbm[0].output_momentum[i]+= rbm[0].learning_rate*dw[0].delta_output_bias[i]/(double)dw[0].batch_size; 
    for(int j=0;j<rbm[0].n_inputs;j++) {
      double step= get_matrix_value(dw[0].delta_w, i, j); // delta_w_i_j
      double previous_w_i_j= get_matrix_value(rbm[0].io_weights, i, j);

      // If using L2 penalty (a.k.a "weight decay"), apply that here.
      if(rbm[0].use_l2_penalty) 
        step-= rbm[0].weight_cost*previous_w_i_j; // Do I apply this to the momentum term as well, or just the correction?!
      step*= rbm[0].learning_rate/(double)dw[0].batch_size; // For the momentum method ... do I still scale by the batch size?!

      // Update weights.  \theta_t = \theta_t' - \epsilon_{t-1} \gradient_f(\theta_{t-1} + \mu_{t-1}v_{t-1}) // (eq. 7.10, 2nd half).
      // \theta_t' was applied before taking the step.
      set_matrix_value(rbm[0].io_weights, i, j, previous_w_i_j+step);  //  

      // Update velocities.  v_t = v_t' - \epsilon_{t-1} \gradient_f(\theta_{t-1} + \mu_{t-1}v_{t-1}) // (eq. 7.11, 2nd half).
      // v_t' was applied before taking the step.
      double previous_momentum_i_j= get_matrix_value(rbm[0].momentum, i, j);
      set_matrix_value(rbm[0].momentum, i, j, previous_momentum_i_j+step);

      if(i==0 && rbm[0].update_input_bias) { // Only update once... and if everything says to update.
        rbm[0].bias_inputs[j]+= rbm[0].learning_rate*dw[0].delta_input_bias[j]/(double)dw[0].batch_size;
        rbm[0].input_momentum[j]+= rbm[0].learning_rate*dw[0].delta_input_bias[j]/(double)dw[0].batch_size;
      }
    }
  }
}

static inline void compute_delta_w(rbm_t *rbm, delta_w_t *batch, double *init_output_recon, double *input_example, double *output_recon, double *input_recon) {
  #pragma omp parallel for
  for(int i=0;i<rbm[0].n_outputs;i++) {
    batch[0].delta_output_bias[i]+= init_output_recon[i]-output_recon[i];
    for(int j=0;j<rbm[0].n_inputs;j++) {
      double delta_w_i_j= get_matrix_value(batch[0].delta_w, i, j)+
			(rbm_sample_state(init_output_recon[i])*input_example[j])-(output_recon[i]*input_recon[j]); // <ViHj_data>-<ViHj_recon>
      set_matrix_value(batch[0].delta_w, i, j, delta_w_i_j);

      if(i==0) // Only 
        batch[0].delta_input_bias[j]+= input_example[j]-input_recon[j]; 
    }
  }
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
static inline void do_batch_member(rbm_t *rbm,  double *input_example, delta_w_t *batch) {
 
  // Run Gibbs sampling for CDn steps.
  double *init_output_recon= (double*)Calloc(rbm[0].n_outputs, double);
  double *input_recon= (double*)Calloc(rbm[0].n_inputs, double);
  clamp_input(rbm, input_example, init_output_recon); // Compute p(hj=1 | v)= logistic_sigmoid(b_j+\sum(v_i * w_ij))
  double *output_recon= vector_copy(init_output_recon, rbm[0].n_outputs);  
  for(int cd=0;cd<rbm[0].cd_n;cd++) {
    clamp_output(rbm, output_recon, input_recon); // Get the input_recon(struction), using the output from the previous step.
    clamp_input(rbm, input_recon, output_recon); // Get the output_recon(struction), using the input from the previous step.
  }
     
  // Put all of this together so that only 1 loop is required ...
  // Compute  ... in this computation, sample random states (?!).
  // Compute <vihj>_data-<vihj>_recon and update batch.
  compute_delta_w(rbm, batch, init_output_recon, input_example, output_recon, input_recon);

  Free(init_output_recon); Free(output_recon); Free(input_recon);
}

/************************************************************************************
 *
 *  Optimize the weights weights over a training dataset using contrastive divergence.
 *
 *  Arguments:
 *    rbm           --> Restricted boltzman machine.
 *    input_example --> A matrix (n_inptus x batch_size) of input examples.  
 *
 *  Following recommendations in: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf (V. 1; dated: Aug. 2nd 2010).
 *
 * Code currently trains by taking the MEAN of gradients in a batch!  This seems most intiutive to me, and more
 *  or less agrees with comments in: http://www.elen.ucl.ac.be/Proceedings/esann/esannpdf/es2012-71.pdf.
 * Don't know what all these how-to pages are talking about when they say 'multiplying matrices', though!?
 */
void *rbm_partial_minibatch(void *ptab) {
  rbm_pthread_arg_t *pta= (rbm_pthread_arg_t*)ptab;
  rbm_t *rbm= pta[0].rbm;
  double *input_example= pta[0].input;
    
  // Compute the \sum gradient over the mini-batch.
  for(int i=0;i<pta[0].do_n_elements;i++) { // Foreach item in the batch.
    do_batch_member(rbm, input_example, pta[0].batch);
    input_example+= rbm[0].n_inputs; // Update the input_example pointer to the next input sample.
  }
  
}
 
void do_minibatch(rbm_t *rbm, double *input_example) { // Use velocity?!; Use sparsity target?!  // Change name to 

  // If using momentum Take a step BEFORE computing the local gradient.
  if(rbm[0].use_momentum) { 
    initial_momentum_step(rbm);
  }

  rbm_pthread_arg_t pta;
  pta.rbm= rbm;
  pta.input= input_example;
  pta.do_n_elements= rbm[0].batch_size;
  pta.batch= alloc_dwt_from_rbm(rbm);
  rbm_partial_minibatch(&pta);
  
  // Take a step in teh direction of the gradient.
  if(rbm[0].use_momentum) { // Correct and update momentum term.
    apply_momentum_correction(rbm, pta.batch); 
  }
  else { // Update weights. \delta w_{ij} = \epislon * (<v_i h_j>_data - <v_i h_j>recon 
    apply_delta_w(rbm, pta.batch);
  }
  
  // Cleanup temporary variables ...  
  free_delta_w_ptr(pta.batch, 1); 
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
void rbm_train(rbm_t *rbm, double *input_example, int n_examples, int n_epocs, int n_threads) {
  omp_set_num_threads(n_threads);

  double *current_position;
  int n_training_iterations= floor(n_examples/rbm[0].batch_size); 
  int left_over_iterations= n_examples%rbm[0].batch_size; 

  for(int i=0;i<n_epocs;i++) {
    //if(i%(100)) Rprintf(".");
    current_position= input_example; // Reset training pointer.
    for(int j=0;j<n_training_iterations;j++) {
      do_minibatch(rbm, current_position);  // Do a minibatch using the current position of the training pointer.
      current_position+= rbm[0].batch_size*rbm[0].n_inputs; // Increment the input_example pointer batch_size # of columns.
	}
	if(left_over_iterations>0) {
      int prev_batch_size = rbm[0].batch_size;
      rbm[0].batch_size= left_over_iterations;
      do_minibatch(rbm,current_position);
      rbm[0].batch_size= prev_batch_size;
    }
  }

}

/************************************************************************************
 *
 *  An R interface for RBM training ...
 */

SEXP train_rbm_R(SEXP rbm_r, SEXP training_data_r, SEXP n_epocs_r, SEXP n_threads_r) {
  rbm_t *rbm= rbm_r_to_c(rbm_r); // Get values from R function.

  int n_examples= Rf_nrows(training_data_r)/rbm[0].n_inputs;
  int n_epocs= INTEGER(n_epocs_r)[0];
  int n_threads= INTEGER(n_threads_r)[0];
  double *input_example= REAL(training_data_r);

  rbm_train(rbm, input_example, n_examples, n_epocs, n_threads);

  return(rbm_r);
}
