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

/* Required to turn of C stack limit. */
#ifndef Win32
#include <Rinterface.h>
#if !defined(HAVE_UINTPTR_T) && !defined(uintptr_t)
typedef unsigned long uintptr_t;
#endif
extern uintptr_t R_CStackLimit; /* C stack limit */
#endif

#endif

