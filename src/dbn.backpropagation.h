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

/* If C stack limit must be turned off. */
/*
#ifndef Win32 // Not sure I can use this without setting it?!  Is it an R global?
#include <Rinterface.h>
#if !defined(HAVE_UINTPTR_T) && !defined(uintptr_t)
typedef unsigned long uintptr_t;
#endif
extern uintptr_t R_CStackLimit; // C stack limit 
//  R_CStackLimit = (uintptr_t)-1; // This snippet actually turns off the stack limit.
#endif
*/
#endif

