/*
 * dbn.receptivefields.c -- For a given output configuration, computes the execpted input layer.
 *
 */
#include "pthread.support.h"
 
#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Applic.h>
#include "dbn.h"
#include "rbm.h"
#include "matrix_functions.h"

#ifdef _POSIX_THREADS 
#include <pthread.h>
#endif

/****************************************************************************
 *
 * Compute.
 * 
 * For a given output, computes the input layer.
 */

static inline double *dbn_recptivefields(dbn_t *dbn, double *output) {
  double *layer_output= vector_copy(output, dbn->n_outputs);

  for(int i=(dbn->n_rbms-1);i>=0;i--) { // Get inputs for each RBM, walking down the network.
    current_input= get_layer_inputs(dbn, i, layer_output, 1);
    Free(layer_output);  // Careful of memory leaks when switching around these pointers!!
    layer_output= current_input;
  }

  return(current_input);
}



/*
 *  Sets the input, and returns the output ...
 */ 
SEXP recptivefields_dbn_R(SEXP dbn_r, SEXP output_r, SEXP n_threads_r) {
  dbn_t *dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  
  int n_threads= INTEGER(n_threads_r)[0];
  double *output= REAL(output_r);
  int n_examples= 1;//Rf_nrows(input_r)/dbn[0].n_inputs;
  int cd_n= INTEGER(R_cd_n)[0];
 
  SEXP input_r;
  protect(input_r= allocMatrix(REALSXP, dbn->n_inputs, n_examples));
  double *input= REAL(input_r);
  
  double *input_recon= dbn_recptivefields(dbn, output);
  
  for(int i=0;i<dbn->n_inputs;i++)
    input[i]= input_recon[i];
	
  Free(input_recon);
/*  
  #ifdef _POSIX_THREADS 
    run_batch_compute_receptivefields(dbn, input, n_examples, n_threads, output);
  #else 
    run_batch_receptivefields(dbn, input, n_examples, n_threads, output);
  #endif
*/
  unprotect(1);
  return(input_r);
}


