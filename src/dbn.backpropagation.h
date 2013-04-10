#ifndef dbn_backpropagation_H 
#define dbn_backpropagation_H
#include "dbn.h"

typedef struct {
  dbn_t *dbn;
  double *input;
  double *expected_output;
  
  int do_n_elements;
  
  delta_w_t *batch;
} dbn_pthread_arg_t;

#endif

