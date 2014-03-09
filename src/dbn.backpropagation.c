/*
 * dbn.backpropagation.c -- This file implements the backpropagation algorithm
 *                          to refine deep belief networks.
 *
 */
#include "pthread.support.h"
 
#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Applic.h>
#include "dbn.h"
#include "dbn.backpropagation.h"
#include "rbm.h"
#include "matrix_functions.h"

#ifdef _POSIX_THREADS 
#include <pthread.h>
#endif

/* 
 * Takes the sum of two delta_w_t variables, batch and dw.  Sum returned in batch. 
 *
 * Previously defined in rbm.c.  Here needed just for backprop.
 */
void sum_delta_w(delta_w_t *batch, delta_w_t *dw) {
  matrix_sum(batch->delta_w, dw->delta_w);
  vector_sum(batch->delta_output_bias, dw->delta_output_bias, batch->delta_w->ncols);
  if(batch->input_bias_allocated && dw->input_bias_allocated)
    vector_sum(batch->delta_input_bias, dw->delta_input_bias, batch->delta_w->nrows);
}



/****************************************************************************
 *
 * Backpropagation.
 * 
 * After initial training, backpropagation is used to refine the network
 * for descriminitive classification.
 *
 * Took most info from Hinton's Coursea lecture on backpropagation ... Last slide in lecture 3d.
 * Here: https://d396qusza40orc.cloudfront.net/neuralnets/lecture_slides%2Flec3.pdf
 * (NOTE: May require Couresea account).
 */

 
double **dbn_compute_store_layers(dbn_t *dbn, double *input) {
  double **layer_output= (double**)Calloc(dbn->n_layers,double*);
  layer_output[0]= vector_copy(input, dbn->n_inputs);
  for(int i=0;i<dbn->n_rbms;i++) {
    layer_output[i+1]= get_layer_outputs(dbn, i, layer_output[i], 1);
  }
  return(layer_output);
}

void compute_weight_errors(dbn_t *dbn, int layer, double **observed_output, double *neuron_error, delta_w_t *batch) {
  int n_outputs_cl= dbn->rbms[layer].n_outputs; // # outputs in current layer
  int n_inputs_cl= dbn->rbms[layer].n_inputs;   // # inputs in current layer

  // Compute error for this layer.
  for(int i=0;i<n_inputs_cl;i++) {
    for(int j=0;j<n_outputs_cl;j++) {
      // Compute error derivites for the weights ... (dE/w_{i,j}).
      double previous_ij= get_matrix_value(batch->delta_w, j, i);
      set_matrix_value(batch->delta_w, j, i, previous_ij+observed_output[layer][i]*neuron_error[j]); 
    }
  }
  
  // Compute error derivites for the biases.  Conceptually similar to a connection with a neuron of constant output (==1).
  // see: http://stackoverflow.com/questions/3775032/how-to-update-the-bias-in-neural-network-backpropagation.
  for(int j=0;j<n_outputs_cl;j++)  batch->delta_output_bias[j]+= neuron_error[j]; //*observed_output (==DEFINED_AS== 1);
}

void compute_next_layer_neuron_error(dbn_t *dbn, int layer, double **observed_output, double *neuron_error, double *next_layer_neuron_error) {
  int n_outputs_cl= dbn->rbms[layer].n_outputs; // # outputs in current layer
  int n_inputs_cl= dbn->rbms[layer].n_inputs;   // # inputs in current layer

  // If we are computing the error for the next layer, do that here.  NOTE: Separate from the previous loop to allow compiler to vectorize.
  for(int i=0;i<n_inputs_cl;i++) {
    next_layer_neuron_error[i]= 0;
    for(int j=0;j<n_outputs_cl;j++) {
      // Compute error for neurons in an internal 'hidden' layer [dE/dy_{i}].
      // dE/dy_{i} = \sum_j w_{i,j}* dE/dz_{j}; where j= \set(outputs); i= \set(inputs).
      next_layer_neuron_error[i]+= neuron_error[j]*get_matrix_value(dbn->rbms[layer].io_weights, j, i);
    }
    next_layer_neuron_error[i]*= observed_output[layer][i]*(1-observed_output[layer][i]);
  }
}
 
/* Returns the error derivitives for a particular example.  Equilavent to do_batch_member in rbm.c. */
void backpropagation(dbn_t *dbn, double *input, double *expected_output, delta_w_t *batch, int compute_only_top_layer) {
  double **observed_output= dbn_compute_store_layers(dbn, input); // Compute the output of the neural network.
  double *next_layer_neuron_error, *neuron_error; // Stores dE/dz.
  
  // Compute last layer error term: dE/dz_j= y_j * (1-y_j) * dE/dy_j
  // Where dE/dy_j= (t_j-y_j) ==> t --> target output.  y--> observed output.
  int n_outputs_ll= dbn->n_outputs; // n_outputs in the last layer.
  int layer_index= dbn->n_layers-1; // Index of the layer in the double **.

  neuron_error= (double*)Calloc(n_outputs_ll,double);
  
  for(int j=0;j<n_outputs_ll;j++) {// Foreach neuron in the output layer.
    double oo= observed_output[layer_index][j];
    neuron_error[j]= oo*(1-oo)*(expected_output[j]-oo); // Compute dE/dz_j
  }
  
  // Makes a single pass over the entire deep belief network.
  // Foreach layer, compute the layer's weight error derivitives (dE/dw) and the next neuron output error term (dE/dz).
  for(int layer=(dbn->n_rbms-1);layer>=0;layer--) {
    int n_inputs_cl= dbn->rbms[layer].n_inputs;   // # inputs in current layer

    compute_weight_errors(dbn, layer, observed_output, neuron_error, &(batch[layer])); 

	if(compute_only_top_layer) { // JUST backpropagate the last layer (the others will be ignored).
      Free(neuron_error);
      break;
	} else if(layer>0 /*&& !compute_only_top_layer*/) { // Compute error in the next layer.
      next_layer_neuron_error= (double*)Calloc(n_inputs_cl,double);
      compute_next_layer_neuron_error(dbn, layer, observed_output, neuron_error, next_layer_neuron_error); 
      Free(neuron_error);
      neuron_error= next_layer_neuron_error;
	} else {
      Free(neuron_error);
	}
	
   }
  
  // Free temporary storage ...
  for(int i=0;i<dbn->n_layers;i++)
    Free(observed_output[i]);
  Free(observed_output);
}

void *dbn_backprop_partial_minibatch(void *ptab) {
  dbn_pthread_arg_t *pta= (dbn_pthread_arg_t*)ptab;
  for(int i=0;i<pta->do_n_elements;i++) {
    backpropagation(pta->dbn, pta->input, pta->expected_output, pta->batch, pta->compute_only_top_layer);

    // Increment pointers.
    pta->input+= pta->dbn->n_inputs;
    pta->expected_output+= pta->dbn->n_outputs;
  }
}

/* Runs the backpropagation algorithm over each element of a mini-batch. */
void backpropagation_minibatch(dbn_t *dbn, double *input, double *expected_output, int update_top_layer_only, int n_threads) {
  dbn_pthread_arg_t pta;
  pta.dbn= dbn;
  pta.input= input;
  pta.expected_output= expected_output;
  pta.do_n_elements= dbn->batch_size;
  pta.batch= alloc_dwt_from_dbn(dbn);
  pta.compute_only_top_layer= update_top_layer_only;

  // If using a momentum, take a step first.
  for(int i=(dbn->n_rbms-1);i>=0;i--) {
    if(dbn->rbms[i].use_momentum) 
      initial_momentum_step(&(dbn->rbms[i]));
	if(update_top_layer_only) break;
  }

  // Do the minibatch.
  dbn_backprop_partial_minibatch(&pta);
  
  // Update the weights.
  for(int i=(dbn->n_rbms-1);i>=0;i--) {
    if(dbn->rbms[i].use_momentum) { 
      apply_momentum_correction(&(dbn->rbms[i]), &(pta.batch[i]));
    }
    else {
      apply_delta_w(&(dbn->rbms[i]), &(pta.batch[i]));
    }
	if(update_top_layer_only) break;
  }
  free_delta_w_ptr(pta.batch, dbn->n_rbms);
}

/* Runs the backpropagation algorithm over each element of a mini-batch. */
#ifdef _POSIX_THREADS 
void backpropagation_minibatch_pthreads(dbn_t *dbn, double *input, double *expected_output, int update_top_layer_only, int n_threads) {

  // If using a momentum, take a step first.
  for(int i=(dbn->n_rbms-1);i>=0;i--) {
    if(dbn->rbms[i].use_momentum) // dbn[0] could result in a segfault, if it disagrees w/ rbm (b/c it won't be init.).
      initial_momentum_step(&(dbn->rbms[i]));
	if(update_top_layer_only) break;
  }

  // If more threads than batch members, just assign each batch member to a spearate thread.
  n_threads= (dbn->batch_size<n_threads)?dbn->batch_size:n_threads;
  int n_per_batch= floor(dbn->batch_size/n_threads);
  int remainder= (dbn->batch_size%n_threads);

  dbn_pthread_arg_t *pta= (dbn_pthread_arg_t*)Calloc(n_threads, dbn_pthread_arg_t);
  pthread_t *threads= (pthread_t*)Calloc(n_threads, pthread_t);
//  pthread_mutex_init(&backpropagation_mutex, NULL);
  for(int i=0;i<n_threads;i++) {
    // Set up data passed to partial_minibatch()
    pta[i].dbn= dbn;
    pta[i].input= input;
    pta[i].expected_output= expected_output;
    pta[i].batch= alloc_dwt_from_dbn(dbn);
    pta[i].do_n_elements= (i<(n_threads-1))?n_per_batch:(n_per_batch+remainder); // For the last thread, only run remaining elements.

    pthread_create(threads+i, NULL, dbn_backprop_partial_minibatch, (void*)(pta+i));

    // Increment pointers for the next thread.
    input+= pta[i].do_n_elements*dbn->n_inputs;
    expected_output+= pta[i].do_n_elements*dbn->n_outputs;
  }

  // Wait for threads to complete, and combine the data into a single vector.
  delta_w_t *batch;
  for(int i=0;i<n_threads;i++) {
    pthread_join(threads[i], NULL);

    // It's ~2x faster to prepare independent batch examples and sum after, than to use a mutex lock.
    if(i==0) {
      batch= pta[i].batch;
    }
    else {
      for(int j=(dbn->n_rbms-1);j>=0;j--) {
        sum_delta_w(&(batch[j]), &(pta[i].batch[j]));
        if(update_top_layer_only) break;
      }
      free_delta_w_ptr(pta[i].batch, dbn->n_rbms);
    }
  }
  Free(pta); Free(threads);
//  pthread_mutex_destroy(&rbm_mutex);

  // Update the weights.
  for(int i=(dbn->n_rbms-1);i>=0;i--) {
    if(dbn->rbms[i].use_momentum) { // dbn[0] could result in a segfault, if it disagrees w/ rbm (b/c it won't be init.).
      apply_momentum_correction(&(dbn->rbms[i]), &(batch[i]));
    }
    else {
      apply_delta_w(&(dbn->rbms[i]), &(batch[i]));
    }
	if(update_top_layer_only) break;
  }
  free_delta_w_ptr(batch, dbn->n_rbms);

}
#endif

/*
 * Refines the deep belief network using backpropagation of error derivitives for a given number of epocs.
 *
 * Arguments: 
 *   dbn           --> Deep belief network -- full specification ...
 *   input_example --> Input example used for training.  Each columns represents a unique training case.
 *   output_example--> Output example used for training.  
 *   n_examples    --> Specifies the number of examples provided.
 *   n_epocs       --> Specifies the number of times the training program loops over the examples.
 *   n_epocs_fixed --> Number of epocs to fix the top level weights.
 *
 * Assumptions: 
 *   --> n_examples is a multiple of dbn.batch_size ... additional examples are ignored.
 */

void dbn_refine(dbn_t *dbn, double *input_example, double *output_example, int n_examples, int n_epocs, int n_epocs_fixed, int n_threads) {
  double *current_input, *current_output;
  int n_training_iterations= floor(n_examples/dbn->batch_size); 
  int left_over= n_examples%dbn->batch_size;
  int update_top_layer_only;
  
  for(int i=0;i<n_epocs;i++) {
    current_input= input_example; // Reset training pointer.
    current_output= output_example;
	update_top_layer_only= i<n_epocs_fixed;
    for(int j=0;j<n_training_iterations;j++) {
      #ifdef _POSIX_THREADS
       backpropagation_minibatch_pthreads(dbn, current_input, current_output, update_top_layer_only, n_threads);  // Do a minibatch using the current position of the training pointer.
      #else 
       backpropagation_minibatch(dbn, current_input, current_output, update_top_layer_only, n_threads);  // Do a minibatch using the current position of the training pointer.
      #endif
      current_input +=dbn->batch_size*dbn->n_inputs; // Increment the input_example pointer batch_size # of columns.
      current_output+=dbn->batch_size*dbn->n_outputs; // Increment the input_example pointer batch_size # of columns.
    }
    if(left_over>0) { // Do remaining training examples.
      int old_batch_size= dbn->batch_size;
      dbn->batch_size= left_over;
      #ifdef _POSIX_THREADS 
       backpropagation_minibatch_pthreads(dbn, current_input, current_output, update_top_layer_only, n_threads);  // Do a minibatch using the current position of the training pointer.
      #else 
       backpropagation_minibatch(dbn, current_input, current_output, update_top_layer_only, n_threads);  // Do a minibatch using the current position of the training pointer.
      #endif
	  dbn->batch_size= old_batch_size;
    }
  }
}

/* labels_to_matrix --> Converts an R factor type into a double* matrix.
 *
 * training_labels_r --> SEXP Integers, from a factor... assumed to be \set: [1,N_outputs], inclusive.
 * n_outputs --> Number of output nodes.  (i.e. # rows in output).
 * 
 * #columns in return double* ==> n_samples
 */
double *labels_to_matrix(SEXP training_labels_r, int n_outputs) {
  int *tlr= INTEGER(training_labels_r);
  int n_samples= Rf_nrows(training_labels_r);
  double *matrix= (double*)R_alloc(n_samples*n_outputs, sizeof(double));
  
  // Make a matrix with a 1 in the appropriate place in each column.
  for(int i=0;i<n_samples;i++) { // Columns.
    for(int j=0;j<n_outputs;j++) { // Rows.
	  matrix[i*n_outputs+j]= ((j+1)==tlr[i])?1:0;
    }
  }
  
  return(matrix);
}

/*
 *  R interface to refining a deep belief network for discriminitive tasks using backpropagation ...
 */ 
SEXP backpropagation_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP training_labels_r, SEXP n_epocs_r, SEXP n_epocs_fix_gen_r, SEXP n_threads_r) {
  dbn_t *dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  dbn->rbms->update_input_bias=0; // Prevent updating input biases during backprop.

  int n_examples= Rf_nrows(training_data_r)/dbn->n_inputs;
  int n_threads= INTEGER(n_threads_r)[0];
  double *input_examples= REAL(training_data_r);
  double *output_examples= labels_to_matrix(training_labels_r, dbn->n_outputs);

  int n_epocs= INTEGER(n_epocs_r)[0];
  int n_epocs_fixed= INTEGER(n_epocs_fix_gen_r)[0];

  dbn_refine(dbn, input_examples, output_examples, n_examples, n_epocs, n_epocs_fixed, n_threads);
  
  return(dbn_r);
}

