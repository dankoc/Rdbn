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
#include <pthread.h>
#include "dbn.h"
#include "rbm.h"
#include "matrix_functions.h"

/*************************************************************************************
 *  Functions of initiatlizing, allocating, and free-ing rbm_t.
 */

/*
 *  Frees an allocated rbm[0].
 */
void free_dbn(dbn_t dbn) {
  for(int l=0;l<dbn.n_rbms;l++)
    free_rbm(dbn.rbms[l]);
  Free(dbn.layer_sizes);
}


/*
 *  Allocates a delta_w_t object for each layer in a deep belief network ...
 */
delta_w_t *alloc_dwt_from_dbn(dbn_t dbn) {
  delta_w_t *batch= (delta_w_t*)Calloc(dbn.n_rbms, delta_w_t);
  
  for(int i=0;i<dbn.n_rbms;i++) {
    int n_outputs_cl= dbn.rbms[i].n_outputs;
    int n_inputs_cl= dbn.rbms[i].n_inputs;

    batch[i].delta_w= alloc_matrix(n_outputs_cl, n_inputs_cl);
    batch[i].delta_output_bias= (double*)Calloc(n_outputs_cl, double);

    init_matrix(batch[i].delta_w, 0);
    init_vector(batch[i].delta_output_bias, n_outputs_cl, 0);

    batch[i].update_input_bias= 0;
    batch[i].batch_size= dbn.batch_size;
    batch[i].learning_rate= dbn.learning_rate;
  }
  return(batch);
}

/*
 * Clamps the input for a given layer.  Returns a new *double with the output of that layer.
 */
double *get_layer_outputs(dbn_t dbn, int layer, double *input, int n_inputs) {
  double *layer_output= (double*)Calloc(dbn.rbms[layer].n_outputs*n_inputs, double);
  double *layer_output_ptr= layer_output;
  for(int i=0;i<n_inputs;i++) { // One-by-one fill in the outputs.
    clamp_input(dbn.rbms[layer], input, layer_output_ptr);
	input+= dbn.rbms[layer].n_inputs; // Increment pointers.
	layer_output_ptr+= dbn.rbms[layer].n_outputs;
  }
  return(layer_output);
}

/****************************************************************************
 *
 * Compute.
 * 
 * For a given input, computes the output layer.
 */
double *dbn_compute(dbn_t dbn, double *input) {
  double *current_input= vector_copy(input, dbn.rbms[0].n_inputs);
  double *layer_output;
  for(int i=0;i<dbn.n_rbms;i++) {
    layer_output= get_layer_outputs(dbn, i, current_input, 1);
    Free(current_input);  // Careful of memory leaks when switching around these pointers!!
    current_input= layer_output;
  }
  return(layer_output);
}

// This version stores intermediate layers, and returns a double**.  
// Perfect for use with backpropagation!
double **dbn_compute_store_layers(dbn_t dbn, double *input) {
  double **layer_output= (double**)Calloc(dbn.n_layers, double*);
  layer_output[0]= vector_copy(input, dbn.rbms[0].n_inputs);
  for(int i=0;i<dbn.n_rbms;i++) {
    layer_output[i+1]= get_layer_outputs(dbn, i, layer_output[i], 1);
  }
  return(layer_output);
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

void compute_layer_error(dbn_t dbn, int layer, double **observed_output, double *neuron_error, double *next_layer_neuron_error, delta_w_t batch) {
  int n_outputs_cl= dbn.rbms[layer].n_outputs; // # outputs in current layer
  int n_inputs_cl= dbn.rbms[layer].n_inputs;   // # inputs in current layer

  // Compute error for this layer.
  for(int i=0;i<n_inputs_cl;i++) {
    if(layer>0) next_layer_neuron_error[i]= 0;
    for(int j=0;j<n_outputs_cl;j++) {
      // Compute error derivites for the weights ... (dE/w_{i,j}).
	  double previous_ij= get_matrix_value(batch.delta_w, j, i);
      set_matrix_value(batch.delta_w, j, i, previous_ij+observed_output[layer][i]*neuron_error[j]); 

      // Compute error derivites for the biases.  Conceptually similar to a connection with a neuron of constant output (==1).
      // see: http://stackoverflow.com/questions/3775032/how-to-update-the-bias-in-neural-network-backpropagation
      // At this time, I am NOT updating input biases using backpropagation
      if(i==0) batch.delta_output_bias[j]+= neuron_error[j]; //*observed_output (==DEFINED_AS== 1);
  
      // Compute error for neurons in an internal 'hidden' layer [dE/dy_{i}].
      // dE/dy_{i} = \sum_j w_{i,j}* dE/dz_{j}; where j= \set(outputs); i= \set(inputs).
      if(layer>0) // No need to compute in the last layer.
        next_layer_neuron_error[i]+= neuron_error[j]*get_matrix_value(dbn.rbms[layer].io_weights, j, i);
    }
  }
}
 
/* Returns the error derivitives for a particular example.  Equilavent to do_batch_member in rbm.c. */
void backpropagation(dbn_t dbn, double *input, double *expected_output, delta_w_t *batch) {
  double **observed_output= dbn_compute_store_layers(dbn, input); // Compute the output of the neural network.

  double *next_layer_neuron_error, *neuron_error; // Stores dE/dz.
  
  // Compute last layer error term: dE/dz_j= y_j * (1-y_j) * dE/dy_j
  // Where dE/dy_j= (t_j-y_j) ==> t --> target output.  y--> observed output.
  int n_outputs_ll= dbn.rbms[dbn.n_rbms-1].n_outputs; // n_outputs in the last layer.
  int layer_index= dbn.n_layers-1; // Index of the layer in the double **.
  neuron_error= (double*)Calloc(n_outputs_ll, double);
//  neuron_errors[layer_index]= (double*)Calloc(n_outputs_ll, double);
  for(int j=0;j<n_outputs_ll;j++) {// Foreach neuron in the output layer.
    double oo= observed_output[layer_index][j];
    neuron_error[j]= oo*(1-oo)*(oo-expected_output[j]); // Compute dE/dz_j
  }
  
  // Makes a single pass over the entire deep belief network.
  // Foreach layer, compute the layer's weight error derivitives (dE/dw) and the next neuron output error term (dE/dz).
  for(int layer=(dbn.n_rbms-1);layer>=0;layer--) {
    int n_inputs_cl= dbn.rbms[layer].n_inputs;   // # inputs in current layer

    if(layer>0) next_layer_neuron_error= (double*)Calloc(n_inputs_cl, double);
    compute_layer_error(dbn, layer, observed_output, neuron_error, next_layer_neuron_error, batch[layer]);     // Shortcut, for code readability.  Computes the error term for the current layer.

    Free(neuron_error);
    if(layer>0) neuron_error= next_layer_neuron_error;
  }
  
  // Free temporary storage ...
  for(int i=0;i<dbn.n_layers;i++)
    Free(observed_output[i]);
  Free(observed_output);
}

void *dbn_backprop_partial_minibatch(void *ptab) {
  dbn_pthread_arg_t *pta= (dbn_pthread_arg_t*)ptab;

  for(int i=0;i<pta[0].do_n_elements;i++) {
    backpropagation(pta[0].dbn, pta[0].input, pta[0].expected_output, pta[0].batch);

    // Increment pointers.
    pta[0].input+= pta[0].dbn.rbms[0].n_inputs;
    pta[0].expected_output+= pta[0].dbn.rbms[pta[0].dbn.n_rbms-1].n_outputs;
  }
}

/////////////IF NO PTREADS, USE THIS. ///////////////////////////////////////////////
/* Runs the backpropagation algorithm over each element of a mini-batch. */
void backpropagation_minibatch(dbn_t dbn, double *input, double *expected_output) {
  dbn_pthread_arg_t pta;
  pta.dbn= dbn;
  pta.input= input;
  pta.expected_output= expected_output;
  pta.do_n_elements= dbn.batch_size;
  pta.batch= alloc_dwt_from_dbn(dbn);
  dbn_backprop_partial_minibatch(&pta);
  
  // Update the weights.
  for(int i=0;i<dbn.n_rbms;i++) {
    apply_delta_w(dbn.rbms[i], pta.batch[i]);
  }
  free_delta_w_ptr(pta.batch, dbn.n_rbms);
}
/////////////\IF NO PTREADS, USE THIS. ///////////////////////////////////////////////

/////////////IF PTREADS, USE THIS. ///////////////////////////////////////////////
/* Runs the backpropagation algorithm over each element of a mini-batch. */
void backpropagation_minibatch_pthreads(dbn_t dbn, double *input, double *expected_output, int n_threads) {

  // Activate each as a separate thread.
  dbn_pthread_arg_t *pta= (dbn_pthread_arg_t*)Calloc(n_threads, dbn_pthread_arg_t);
  pthread_t *threads= (pthread_t*)Calloc(n_threads, pthread_t);
  int n_per_batch= floor(dbn.batch_size/n_threads);
  int remainder= (dbn.batch_size%n_threads==0)?n_per_batch:(dbn.batch_size%n_threads);
  for(int i=0;i<n_threads;i++) {
    // Set up data passed to partial_minibatch()
    pta[i].dbn= dbn;
    pta[i].input= input;
    pta[i].expected_output= expected_output;
    pta[i].batch= alloc_dwt_from_dbn(dbn);
    pta[i].do_n_elements= (i<(n_threads-1))?n_per_batch:remainder; // For the last thread, only run remaining elements.
	  
    pthread_create(threads+i, NULL, dbn_backprop_partial_minibatch, (void*)(pta+i));
	
	// Increment pointers for the next thread.
	input+= pta[i].do_n_elements*dbn.rbms[0].n_inputs;
	expected_output+= pta[i].do_n_elements*dbn.rbms[dbn.n_rbms-1].n_outputs;
  }

  // Wait for threads to complete, and combine the data into a single vector.
  delta_w_t *batch;
  for(int i=0;i<n_threads;i++) {
    pthread_join(threads[i], NULL);
	
    if(i==0) {
      batch= pta[i].batch;
    }
    else {
      for(int j=0;j<dbn.n_rbms;j++) {
        sum_delta_w(batch[j], pta[i].batch[j]);
      }
      free_delta_w_ptr(pta[i].batch, dbn.n_rbms);
    }
  }
  Free(pta); Free(threads);
 
  // Update the weights.
  for(int i=0;i<dbn.n_rbms;i++) {
    apply_delta_w(dbn.rbms[i], batch[i]);
  }
  free_delta_w_ptr(batch, dbn.n_rbms);
}
/////////////\IF PTREADS, USE THIS. ///////////////////////////////////////////////

/*
 * Refines the deep belief network using backpropagation of error derivitives for a given number of epocs.
 *
 * Arguments: 
 *   dbn           --> Deep belief network -- full specification ...
 *   input_example --> Input example used for training.  Each columns represents a unique training case.
 *   output_example--> Output example used for training.  
 *   n_examples    --> Specifies the number of examples provided.
 *   n_epocs       --> Specifies the number of times the training program loops over the examples.
 *
 * Assumptions: 
 *   --> n_examples is a multiple of dbn.batch_size ... additional examples are ignored.
 */

void dbn_refine(dbn_t dbn, double *input_example, double *output_example, int n_examples, int n_epocs, int n_threads) {
  double *current_input, *current_output;
  int n_training_iterations= floor(n_examples/dbn.batch_size); 
  
  for(int i=0;i<n_epocs;i++) {
    current_input= input_example; // Reset training pointer.
    current_output= output_example;
    for(int j=0;j<n_training_iterations;j++) {
      backpropagation_minibatch_pthreads(dbn, current_input, current_output, n_threads);  // Do a minibatch using the current position of the training pointer.
      current_input+= dbn.batch_size*dbn.rbms[0].n_inputs; // Increment the input_example pointer batch_size # of columns.
      current_output+=dbn.batch_size*dbn.rbms[dbn.n_rbms-1].n_outputs; // Increment the input_example pointer batch_size # of columns.
	}
  }
}
 
/************************************************************************************
 * 
 * Pre-training functions.
 */

/*
 *  Trains a deep belief network, layer-by-layer.
 *
 *  Arguments:
 *    dbn        --> Full representation of the deep belief network.
 *    examples   --> Examples of input used to train the network.
 *    n_examples --> Number of examples passed to the training.
 *    n_epocs    --> Number of times to iterate over the input examples during training.
 */
void dbn_train(dbn_t dbn, double *examples, int n_examples, int n_epocs, int n_threads) {
  // Trian the first layer.
  rbm_train(dbn.rbms[0], examples, n_examples, n_epocs, n_threads);

  // train later layers.
  double *previous_layer_input, *next_layer_input;
  previous_layer_input= examples;
  for(int layer=1;layer<dbn.n_rbms;layer++) {
    next_layer_input= get_layer_outputs(dbn, layer-1, previous_layer_input, n_examples); // Get the output from the previous layer; that's the input to the next layer ...
    rbm_train(dbn.rbms[layer], next_layer_input, n_examples, n_epocs, n_threads); // Train the current layer.
  
    // Free the previous input layer (unless we're on the first pass and pointing to *examples.
	if(layer>1) // DO NOT free *examples.
      Free(previous_layer_input); // Free the previous input layer.
	previous_layer_input= next_layer_input; // Set previous_layer_input to the current layer for the next pass...
  }
  if(dbn.n_rbms>1) // Clean up allocated memory ... iff it's not still set to *examples.
    Free(previous_layer_input); 
}

/*
 *  Converts R S4 object to our C struct ...
 */ 
dbn_t dbn_r_to_c(SEXP dbn_r) {
  dbn_t dbn;//= (dbn_t*)R_alloc(1, sizeof(dbn_t));

  dbn.learning_rate= REAL(GET_SLOT(dbn_r, Rf_install("learning_rate")))[0];;
  dbn.batch_size= INTEGER(GET_SLOT(dbn_r, Rf_install("batch_size")))[0];
 
  dbn.n_layers= INTEGER(GET_SLOT(dbn_r,Rf_install("n_layers")))[0];
  dbn.n_rbms= dbn.n_layers-1;
  dbn.layer_sizes= INTEGER(GET_SLOT(dbn_r, Rf_install("layer_sizes")));

  dbn.n_rbms= dbn.n_layers-1;
  dbn.rbms= (rbm_t*)R_alloc(dbn.n_rbms, sizeof(rbm_t));
  dbn.rbms[0]= rbm_r_to_c(VECTOR_ELT(GET_SLOT(dbn_r, Rf_install("network")), 0)); // Apply rbm_r_to_c for network[[i]].

  for(int i=1;i<dbn.n_rbms;i++) {
    // Force bias_input[i] = bias_output[i-1];
    dbn.rbms[i]= rbm_layer_r_to_c(VECTOR_ELT(GET_SLOT(dbn_r, Rf_install("network")), i), dbn.rbms[i-1].bias_outputs);
  }

 return(dbn);
}

/*
 *  R interface to training a deep belief network ...
 */ 
SEXP train_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP n_epocs_r, SEXP n_threads_r) {
  dbn_t dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  
  int n_examples= Rf_nrows(training_data_r)/dbn.rbms[0].n_inputs;
  double *examples= REAL(training_data_r);
  int n_threads= INTEGER(n_threads_r)[0];

  int n_epocs= INTEGER(n_epocs_r)[0];
  
  dbn_train(dbn, examples, n_examples, n_epocs, n_threads);
  return(dbn_r);
}

/*
 *  R interface to refining a deep belief network for discriminitive tasks using backpropagation ...
 */ 
SEXP refine_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP training_labels_r, SEXP n_epocs_r, SEXP n_threads_r) {
  dbn_t dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  
  int n_examples= Rf_nrows(training_data_r)/dbn.rbms[0].n_inputs;
  int n_threads= INTEGER(n_threads_r)[0];
  double *input_examples= REAL(training_data_r);
  double *output_examples= REAL(training_labels_r);

  int n_epocs= INTEGER(n_epocs_r)[0];

  dbn_refine(dbn, input_examples, output_examples, n_examples, n_epocs, n_threads);
  
  return(dbn_r);
}


