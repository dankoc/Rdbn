#ifndef rbm_train_H 
#define rbm_train_H 
#include "rbm.h"

typedef struct {
  rbm_t *rbm;
  double *input;
  
  int do_n_elements;
  
  delta_w_t *batch;  // Return value...
} rbm_pthread_arg_t;

#endif

