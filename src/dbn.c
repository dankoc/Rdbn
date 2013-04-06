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

/*
 *  Trains a deep belief network, layer-by-layer.
 *
 *  Arguments:
 *    dbn        --> Full representation of the deep belief network.
 *    examples   --> Examples of input used to train the network.
 *    n_examples --> Number of examples passed to the training.
 *    n_epocs    --> Number of times to iterate over the input examples during training.
 */
void dbn_train(dbn_t dbn, double *examples, int n_examples, int n_epocs) {
  // Trian the first layer.
  rbm_train(dbn.rbms[0], examples, n_examples, n_epocs);

  // train later layers.
  double *previous_layer_input, *next_layer_input;
  previous_layer_input= examples;
  for(int layer=1;layer<dbn.n_rbms;layer++) {
    next_layer_input= get_layer_outputs(dbn, layer-1, previous_layer_input, n_examples); // Get the output from the previous layer; that's the input to the next layer ...
    rbm_train(dbn.rbms[layer], next_layer_input, n_examples, n_epocs); // Train the current layer.
  
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

  dbn.n_layers= INTEGER(GET_SLOT(dbn_r,Rf_install("n_layers")))[0];
  dbn.n_rbms= dbn.n_layers-1;
  dbn.layer_sizes= INTEGER(GET_SLOT(dbn_r, Rf_install("layer_sizes")));

  dbn.n_rbms= dbn.n_layers-1;
  dbn.rbms= (rbm_t*)R_alloc(dbn.n_rbms, sizeof(rbm_t));
  for(int i=0;i<dbn.n_rbms;i++) {
    dbn.rbms[i]= rbm_r_to_c(VECTOR_ELT(GET_SLOT(dbn_r, Rf_install("network")), i)); // Apply rbm_r_to_c for network[[i]].
  }

 return(dbn);
}

/*
 *  R interface to training a deep belief network ...
 */ 
SEXP train_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP n_epocs_r) {
  dbn_t dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  
  int n_examples= Rf_nrows(training_data_r)/dbn.rbms[0].n_inputs;
  double *examples= REAL(training_data_r);

  int n_epocs= INTEGER(n_epocs_r)[0];
  
  dbn_train(dbn, examples, n_examples, n_epocs);
  return(dbn_r);
}


