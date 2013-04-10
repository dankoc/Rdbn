#ifndef dbn_compute_H 
#define dbn_compute_H
#include "dbn.h"

typedef struct {
  dbn_t *dbn;        // Deep belief network.
  double *input;     // Inputs to the dbn.
  double *output;    // Pointer to output vector.  results are placed here.
  
  int do_n_elements; // Number of elements for the thread to compute.
} dbn_pthread_predict_arg_t;

#endif

