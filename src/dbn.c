/*
 * dbn.c -- Fast code to learn and use deep belief networks in R/C.
 * 
 * Contains common functions for deep belief networks...  
 * Also functions to pre-train the network by contrastive divergence.
 *
 * Additional spcific functions are divided into spearate files, including: 
 * 
 *  dbn.backpropagation.c --> Implements the backpropagation algorithm for refining deep belief networks.
 *  dbn.compute.c  --> Functions for computing network output from a particular input.
 *
 */
#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Applic.h>
#include "dbn.h"
#include "rbm.h"
#include "matrix_functions.h"


/*
 * Shortcut to allocate memory and clamps the given input layer.  Returns a new *double with the output of that layer.
 */
double *get_layer_outputs(dbn_t *dbn, int layer, double *input, int n_inputs) {
  double *layer_output= (double*)Calloc(dbn->rbms[layer].n_outputs*n_inputs,double);
  double *layer_output_ptr= layer_output;
  for(int i=0;i<n_inputs;i++) { // One-by-one fill in the outputs.
    clamp_input(&(dbn->rbms[layer]), input, layer_output_ptr);
    input+= dbn->rbms[layer].n_inputs; // Increment pointers.
    layer_output_ptr+= dbn->rbms[layer].n_outputs;
  }
  return(layer_output);
}

/*
 * Shortcut to allocate memory and clamps the given input layer.  Returns a new *double with the output of that layer.
 */
double *get_layer_inputs(dbn_t *dbn, int layer, double *output, int n_outputs) {
  double *layer_input= (double*)Calloc(dbn->rbms[layer].n_inputs*n_outputs,double);
  double *layer_input_ptr= layer_input;
  for(int i=0;i<n_outputs;i++) { // One-by-one fill in the outputs.
    clamp_output(&(dbn->rbms[layer]), output, layer_input_ptr);
    output+= dbn->rbms[layer].n_outputs; // Increment pointers.
    layer_input_ptr+= dbn->rbms[layer].n_inputs;
  }
  return(layer_input);
}

/*************************************************************************************
 *  Functions of initiatlizing, allocating, and free-ing rbm_t.
 */

/*
 *  Frees an allocated rbm[0].
 */
void free_dbn(dbn_t *dbn) {
  for(int l=0;l<dbn[0].n_rbms;l++)
    free_rbm(dbn[0].rbms);
  Free(dbn[0].layer_sizes);
  Free(dbn);
}


/*
 *  Allocates a delta_w_t object for each layer in a deep belief network ...
 */
delta_w_t *alloc_dwt_from_dbn(dbn_t *dbn) {
  delta_w_t *batch= (delta_w_t*)Calloc(dbn[0].n_rbms, delta_w_t);
  
  for(int i=0;i<dbn[0].n_rbms;i++) {
    int n_outputs_cl= dbn[0].rbms[i].n_outputs;
    int n_inputs_cl= dbn[0].rbms[i].n_inputs;

    batch[i].delta_w= alloc_matrix(n_outputs_cl, n_inputs_cl);
    batch[i].delta_output_bias= (double*)Calloc(n_outputs_cl, double);

    init_matrix(batch[i].delta_w, 0.0);
    init_vector(batch[i].delta_output_bias, n_outputs_cl, 0.0);

    batch[i].input_bias_allocated= 0;
    batch[i].batch_size= dbn[0].batch_size;
  }
  return(batch);
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
 *    n_threads  --> Number of threads to run pre-training on.
 */
void dbn_train(dbn_t *dbn, double *examples, int n_examples, int n_epocs, int n_threads) {
  // Trian the first layer.
  Rprintf("Training layer 1: ");
  rbm_train(&(dbn->rbms[0]), examples, n_examples, n_epocs, n_threads);
  Rprintf("\n");
  
  // train later layers.
  double *previous_layer_input, *next_layer_input;
  previous_layer_input= examples;
  for(int layer=1;layer<dbn->n_rbms;layer++) {
   next_layer_input= get_layer_outputs(dbn, layer-1, previous_layer_input, n_examples); // Get the output from the previous layer; that's the input to the next layer ...
   Rprintf("Training layer %d: ",layer+1);
   rbm_train(&(dbn->rbms[layer]), next_layer_input, n_examples, n_epocs, n_threads); // Train the current layer.
   Rprintf("\n");

    // Free the previous input layer (unless we're on the first pass and pointing to *examples.
	if(layer>1) // DO NOT free *examples.
      Free(previous_layer_input); // Free the previous input layer.
	previous_layer_input= next_layer_input; // Set previous_layer_input to the current layer for the next pass...
  }
  if(dbn->n_rbms>1) // Clean up allocated memory ... iff it's not still set to *examples.
    Free(previous_layer_input); 
}

/*
 *  Converts R S4 object to our C struct ...
 */ 
dbn_t *dbn_r_to_c(SEXP dbn_r) {
  dbn_t *dbn= (dbn_t*)R_alloc(1, sizeof(dbn_t));

  dbn->batch_size= INTEGER(GET_SLOT(dbn_r, Rf_install("batch_size")))[0];
  dbn->n_layers= INTEGER(GET_SLOT(dbn_r,Rf_install("n_layers")))[0];
  dbn->n_rbms= dbn->n_layers-1;
  dbn->layer_sizes= (long long *)INTEGER(GET_SLOT(dbn_r, Rf_install("layer_sizes")));

  dbn->n_rbms= dbn->n_layers-1;
  dbn->rbms= (rbm_t*)R_alloc(dbn->n_rbms, sizeof(rbm_t));
  dbn->rbms[0]= (rbm_r_to_c(VECTOR_ELT(GET_SLOT(dbn_r, Rf_install("network")), 0)))[0]; // Apply rbm_r_to_c for network[[i]].

  for(int i=1;i<dbn->n_rbms;i++) {
    // Force bias_input[i] = bias_output[i-1];
    dbn->rbms[i]= (rbm_layer_r_to_c(VECTOR_ELT(GET_SLOT(dbn_r, Rf_install("network")), i), dbn->rbms[i-1].bias_outputs))[0];
  }

  dbn->n_outputs= dbn->rbms[dbn->n_rbms-1].n_outputs;
  dbn->n_inputs= dbn->rbms->n_inputs;
  
  return(dbn);
}
 
/*
 *  R interface to pre-training a deep belief network ...
 */ 
SEXP train_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP n_epocs_r, SEXP n_threads_r) {
  dbn_t *dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  
  int n_examples= Rf_nrows(training_data_r)/dbn[0].n_inputs;
  double *examples= REAL(training_data_r);
  int n_threads= INTEGER(n_threads_r)[0];

  int n_epocs= INTEGER(n_epocs_r)[0];
  
  dbn_train(dbn, examples, n_examples, n_epocs, n_threads);
  return(dbn_r);
}
