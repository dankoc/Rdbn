#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Applic.h>
#include <pthread.h>
#include "dbn.h"
#include "dbn.compute.h"
#include "rbm.h"
#include "matrix_functions.h"

/****************************************************************************
 *
 * Compute.
 * 
 * For a given input, computes the output layer.
 */
double *dbn_compute(dbn_t *dbn, double *input) {
  double *current_input= vector_copy(input, dbn[0].rbms[0].n_inputs);
  double *layer_output;
  for(int i=0;i<dbn[0].n_rbms;i++) {
    layer_output= get_layer_outputs(dbn, i, current_input, 1);
    Free(current_input);  // Careful of memory leaks when switching around these pointers!!
    current_input= layer_output;
  }
  return(layer_output);
}

void *batch_compute(void* compute) {

}


/*
 *  Sets the input, and returns the output ...
 */ 
SEXP predict_dbn_R(SEXP dbn_r, SEXP input_r, SEXP n_threads_r) {
  dbn_t *dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  
  int n_threads= INTEGER(n_threads_r)[0];
  double *input= REAL(input_r);

  int n_epocs= INTEGER(n_epocs_r)[0];

  dbn_compute(dbn, input);
  
  return(dbn_r);
}


