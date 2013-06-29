#ifndef deep_belief_network_H 
#define deep_belief_network_H
#include "rbm.h"

typedef struct {
  int n_layers;      // Number of layers in the data ...
  int *layer_sizes;  // Number of nodes in the upper 'output' layer.

  int n_rbms;        // For convenience ... it's ALWAYS= n_layers-1 ... or it's wrong.
  rbm_t *rbms;       // Pointer to a layer of restricted boltzman machines.
  
  int n_outputs;     // Number of output nodes in the network. For convenience.
  int n_inputs;      // Number of input nodes in the network.  Again, for convenience.
  
  // Learning functions ... for use with refining network weights by backpropagation.
  int batch_size;         // Specifies the batch size for training during backprop.
} dbn_t;

/*
 * Shortcut to allocate memory and clamps the given input layer.  Returns a new *double with the output of that layer.
 */

#ifndef INLINE
# if __GNUC__ && !__GNUC_STDC_INLINE__
#  define INLINE extern inline
# else
#  define INLINE inline
# endif
#endif
INLINE double *get_layer_outputs(dbn_t *dbn, int layer, double *input, int n_inputs) {
  double *layer_output= (double*)Calloc(dbn[0].rbms[layer].n_outputs*n_inputs,double);
  double *layer_output_ptr= layer_output;
  for(int i=0;i<n_inputs;i++) { // One-by-one fill in the outputs.
    clamp_input(&(dbn[0].rbms[layer]), input, layer_output_ptr);
	input+= dbn[0].rbms[layer].n_inputs; // Increment pointers.
	layer_output_ptr+= dbn[0].rbms[layer].n_outputs;
  }
  return(layer_output);
}

SEXP train_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP n_epocs_r, SEXP n_threads_r);
SEXP backpropagation_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP training_labels_r, SEXP n_epocs_r, SEXP n_threads_r);
SEXP predict_dbn_R(SEXP dbn_r, SEXP input_r, SEXP n_threads_r);
SEXP convert_to_max_R(SEXP dbn_r, SEXP matrix_r);

delta_w_t *alloc_dwt_from_dbn(dbn_t *dbn);
dbn_t *dbn_r_to_c(SEXP dbn_r);

#ifndef Win32
#include <pthread.h>
pthread_mutex_t backpropagation_mutex;
#endif

#endif

