#ifndef deep_belief_network_H 
#define deep_belief_network_H
#include "rbm.h"

typedef struct {
  int n_layers;      // Number of layers of neurons in the data ...
  long *layer_sizes;  // Number of nodes in the upper 'output' layer.

  int n_rbms;        // For convenience ... it's ALWAYS= n_layers-1 ... or it's wrong.
  rbm_t *rbms;       // Pointer to a layer of restricted boltzman machines.
  
  long n_outputs;     // Number of output nodes in the network. For convenience.
  long n_inputs;      // Number of input nodes in the network.  Again, for convenience.
  
  // Learning functions ... for use with refining network weights by backpropagation.
  int batch_size;         // Specifies the batch size for training during backprop.
} dbn_t;

/*
 * Shortcut to allocate memory and clamps the given input layer.  Returns a new *double with the output of that layer.
 */

double *get_layer_outputs(dbn_t *dbn, int layer, double *input, int n_inputs);
double *get_layer_inputs(dbn_t *dbn, int layer, double *output, int n_outputs);
double *dbn_daydream(dbn_t *dbn, int cd_n, double *input);
double *dbn_clamplayer(dbn_t *dbn, double *output, int layer);

SEXP train_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP n_epocs_r, SEXP n_threads_r);
SEXP backpropagation_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP training_labels_r, SEXP n_epocs_r, SEXP n_epocs_fix_gen_r, SEXP n_threads_r);
SEXP predict_dbn_R(SEXP dbn_r, SEXP input_r, SEXP n_threads_r);
SEXP convert_to_max_R(SEXP dbn_r, SEXP matrix_r);
SEXP daydream_dbn_R(SEXP dbn_r, SEXP input_r, SEXP R_cd_n, SEXP n_threads_r);
SEXP clamplayer_dbn_R(SEXP dbn_r, SEXP output_r, SEXP layer_r, SEXP n_threads_r);

delta_w_t *alloc_dwt_from_dbn(dbn_t *dbn);
dbn_t *dbn_r_to_c(SEXP dbn_r);

#ifdef _POSIX_THREADS 
#include <pthread.h>
pthread_mutex_t backpropagation_mutex;
#endif

#endif

