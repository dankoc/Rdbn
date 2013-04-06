#ifndef deep_belief_network_H 
#define deep_belief_network_H
#include "rbm.h"

typedef struct {
  int n_layers;      // Number of layers in the data ...
  int *layer_sizes;  // Number of nodes in the upper 'output' layer.

  int n_rbms;        // For convenience ... it's ALWAYS= n_layers-1 ... or it's wrong.
  rbm_t *rbms;       // Pointer to a layer of restricted boltzman machines.
} dbn_t;

SEXP train_dbn_R(SEXP dbn_r, SEXP training_data_r, SEXP n_epocs_r);

#endif

