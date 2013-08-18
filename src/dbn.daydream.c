/*
 * dbn.backpropagation.c -- This file contains the code to compute outputs
 *                          for a trained network.
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
 * For a given input, computes the output layer.
 */

static inline double *dbn_daydream(dbn_t *dbn, int cd_n, double *input) {
  double *current_input= vector_copy(input, dbn[0].n_inputs);
  double *layer_output;
  for(int i=0;i<(dbn[0].n_rbms-1);i++) { // Get outputs for each RBM, walking up the network.
    layer_output= get_layer_outputs(dbn, i, current_input, 1);
    Free(current_input);  // Careful of memory leaks when switching around these pointers!!
    current_input= layer_output;
  }
  
  double *recon_input= daydream(&(dbn->rbms[dbn->n_rbms-1]), cd_n, layer_output);
  Free(layer_output);
  
  layer_output= recon_input;
  for(int i=(dbn[0].n_rbms-2);i>=0;i--) { // Get inputs for each RBM, walking down the network.
    current_input= get_layer_inputs(dbn, i, layer_output, 1);
    Free(layer_output);  // Careful of memory leaks when switching around these pointers!!
    layer_output= current_input;
  }

  return(current_input);
}



/*
 *  Sets the input, and returns the output ...
 */ 
SEXP daydream_dbn_R(SEXP dbn_r, SEXP input_r, SEXP R_cd_n, SEXP n_threads_r) {
  dbn_t *dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  
  int n_threads= INTEGER(n_threads_r)[0];
  double *input= REAL(input_r);
  int n_examples= 1;//Rf_nrows(input_r)/dbn[0].n_inputs;
  int cd_n= INTEGER(R_cd_n)[0];
 
  SEXP output_r;
  protect(output_r= allocMatrix(REALSXP, dbn->n_inputs, n_examples));
  double *output= REAL(output_r);
  
  double *input_recon= dbn_daydream(dbn, cd_n, input);
  
  for(int i=0;i<dbn->n_inputs;i++)
    output[i]= input_recon[i];
	
  Free(input_recon);
/*  
  #ifdef _POSIX_THREADS 
    run_batch_compute_daydream(dbn, input, n_examples, n_threads, output);
  #else 
    run_batch_daydream(dbn, input, n_examples, n_threads, output);
  #endif
*/
  unprotect(1);
  return(output_r);
}


