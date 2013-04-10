#ifndef dbn_compute_H 
#define dbn_compute_H
#include "dbn.h"

typedef struct {
  dbn_t *dbn;
  double *input;
  double *expected_output;
  
  int do_n_elements;
  
  delta_w_t *batch;
} dbn_pthread_predict_arg_t;

#endif

