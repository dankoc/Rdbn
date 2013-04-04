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
void free_dbn(dbn_t *dbn) {
  for(int l=0;l<dbn[0].n_rbms;l++)
    free_rbm(dbn[0].rbms+l);
  Free(dbn[0].layer_sizes);
  Free(dbn);
}

/*
 * Clamps the input for a given layer.  Returns a new *double with the output of that layer.
 */
double *get_layer_outputs(dbn_t *dbn, double *input, int layer) {
  double *layer_output= (double*)Calloc(dbn[0].rbms[layer].n_outputs, double);
  clamp_input(dbn[0].rbms+layer, input, layer_output);
  return(layer_output);
}

/*
 *  Trains a deep belief network, layer-by-layer.
 */
void dbn_train(dbn_t *dbn, double *examples) {
  // Trian the first layer.
  train(dbn[0].rbms+0, examples);

  // train later layers.
  double *previous_layer_input, *next_layer_input;
  previous_layer_input= examples;
  for(int layer=1;layer<dbn[0].n_rbms;layer++) {
    next_layer_input= get_layer_outputs(dbn, previous_layer_input, layer-1); // Get the output from the previous layer; that's the input to the next layer ...
    train(dbn[0].rbms+layer, next_layer_input); // Train the current layer.
  
    // Free the previous input layer (unless we're on the first pass and pointing to *examples.
	if(layer>1) // DO NOT free *examples.
      Free(previous_layer_input); // Free the previous input layer.
	previous_layer_input= next_layer_input; // Set previous_layer_input to the current layer for the next pass...
  }
  if(dbn[0].n_rbms>1) // Clean up allocated memory ... iff it's not still set to *examples.
    Free(previous_layer_input); 
}

/*
 *  Converts R S4 object to our C struct ...
 */ 
dbn_t *dbn_r_to_c(SEXP dbn_r) {
  dbn_t *dbn= (dbn_t*)R_alloc(1, sizeof(dbn_t));

  dbn[0].n_layers= INTEGER(GET_SLOT(dbn_r,Rf_install("n_layers")))[0];
  dbn[0].layer_sizes= INTEGER(GET_SLOT(dbn_r, Rf_install("layer_sizes")));

  dbn[0].n_rbms= dbn[0].n_layers-1;
  dbn[0].rbms= (rbm_t*)R_alloc(dbn[0].n_rbms, sizeof(rbm_t));
  for(int i=0;i<dbn[0].n_rbms;i++) {
    dbn[0].rbms[i]= rbm_r_to_c(VECTOR_ELT(GET_SLOT(dbn_r, Rf_install("network")), i)); // Apply rbm_r_to_c for network[[i]].
  }
  return(dbn);
}

/*
 *  R interface to training a deep belief network ...
 */ 
SEXP train_dbn_R(SEXP dbn_r, SEXP training_data_r) {
  dbn_t *dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  
  double *examples= REAL(training_data_r);
  dbn_train(dbn, examples);
  return(dbn_r);
}


