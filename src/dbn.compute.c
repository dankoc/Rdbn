/*
 * dbn.backpropagation.c -- This file contains the code to compute outputs
 *                          for a trained network.
 *
 */
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
 
// This version stores intermediate layers, and returns a double**.  
// Perfect for use with backpropagation!
double **dbn_compute_store_layers(dbn_t *dbn, double *input) {
  double **layer_output= (double**)Calloc(dbn[0].n_layers, double*);
  layer_output[0]= vector_copy(input, dbn[0].n_inputs);
  for(int i=0;i<dbn[0].n_rbms;i++) {
    layer_output[i+1]= get_layer_outputs(dbn, i, layer_output[i], 1);
  }
  return(layer_output);
}

double *dbn_compute(dbn_t *dbn, double *input) {
  double *current_input= vector_copy(input, dbn[0].n_inputs);
  double *layer_output;
  for(int i=0;i<dbn[0].n_rbms;i++) {
    layer_output= get_layer_outputs(dbn, i, current_input, 1);
    Free(current_input);  // Careful of memory leaks when switching around these pointers!!
    current_input= layer_output;
  }
  return(layer_output);
}

void *batch_compute(void* compute) {
  dbn_pthread_predict_arg_t *pta= (dbn_pthread_predict_arg_t*)compute;

  for(int i=0;i<pta[0].do_n_elements;i++) {
    // Get the dbn outputs.
    double *output= dbn_compute(pta[0].dbn, pta[0].input);
	
	// Copy to output vector.
    for(int j=0;j<pta[0].dbn[0].n_outputs;j++) {
      pta[0].output[j]= output[j];
    }
	
    // Clean up and increment pointers for the next pass.
    Free(output);
    pta[0].output += pta[0].dbn[0].n_outputs;
    pta[0].input += pta[0].dbn[0].n_inputs;
  }
}

void run_batch_compute_pthreads(dbn_t *dbn, double *input, int n_examples, int n_threads, double *output) {
  // Activate each as a separate thread.
 // If more threads than batch members, just assign each batch member to a spearate thread.
  n_threads= (dbn[0].batch_size<n_threads)?dbn[0].batch_size:n_threads;
  int n_per_batch= floor(dbn[0].batch_size/n_threads);
  int remainder= (dbn[0].batch_size%n_threads==0)?n_per_batch:(dbn[0].batch_size%n_threads);
  	  
  dbn_pthread_predict_arg_t *pta= (dbn_pthread_predict_arg_t*)Calloc(n_threads, dbn_pthread_predict_arg_t);
  pthread_t *threads= (pthread_t*)Calloc(n_threads, pthread_t);
  for(int i=0;i<n_threads;i++) {
    // Set up data passed to partial_minibatch()
    pta[i].dbn= dbn;
    pta[i].input= input;
    pta[i].output= output;
    pta[i].do_n_elements= (i<(n_threads-1))?n_per_batch:(remainder); // For the last thread, only run remaining elements.
    Rprintf("nelem: %d\n", pta[i].do_n_elements);
    pthread_create(threads+i, NULL, batch_compute, (void*)(pta+i));
	
	// Increment pointers for the next thread.
    input+= pta[i].do_n_elements*dbn[0].n_inputs;
    output+= pta[i].do_n_elements*dbn[0].n_outputs;
  }
  
  // Wait for threads to complete.
  for(int i=0;i<n_threads;i++) {
    pthread_join(threads[i], NULL);
  }
  
  // Cleanup & return!  Output data should already be set in the proper places.
  Free(pta);
  Free(threads);
}

void run_batch_compute(dbn_t *dbn, double *input, int n_examples, int n_threads, double *output) {
  dbn_pthread_predict_arg_t pta;
  pta.dbn= dbn;
  pta.input= input;
  pta.output= output;
  pta.do_n_elements= n_examples; 
  batch_compute(&pta);
}


void convert_to_max(dbn_t *dbn, double *matrix, double *vector, int n_observations) {
  double maxVal; 
  int maxIndx;
  for(int o=0;o<n_observations;o++) {
    maxVal= -1; maxIndx= 0;
    for(int i=0;i<dbn[0].n_outputs;i++) {
      if(matrix[o*dbn[0].n_outputs+i]> maxVal) {
	    maxVal= matrix[o*dbn[0].n_outputs+i];
		maxIndx= i+1;
      }
    }
    vector[o]= maxIndx;
  }
}

/*
 *  Takes in a matrix of predictions, and converts to a vector of the most likely class for each occurence ...
 */ 
SEXP convert_to_max_R(SEXP dbn_r, SEXP matrix_r) {
  dbn_t *dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  double *matrix= REAL(matrix_r);
  int n_obs= Rf_nrows(matrix_r)/dbn[0].n_outputs;
  
  SEXP output_r;
  protect(output_r= allocVector(REALSXP, n_obs));
  double *output= REAL(output_r);
  
  convert_to_max(dbn, matrix, output, n_obs);
  
  unprotect(1);
  return(output_r);
}

/*
 *  Sets the input, and returns the output ...
 */ 
SEXP predict_dbn_R(SEXP dbn_r, SEXP input_r, SEXP n_threads_r) {
  dbn_t *dbn= dbn_r_to_c(dbn_r); // Get values from R function.
  
  int n_threads= INTEGER(n_threads_r)[0];
  double *input= REAL(input_r);
  int n_examples= Rf_nrows(input_r)/dbn[0].n_inputs;
 
  SEXP output_r;
  protect(output_r= allocMatrix(REALSXP, dbn[0].n_outputs, n_examples));
  double *output= REAL(output_r);
  
  if(n_threads > 1)
    run_batch_compute_pthreads(dbn, input, n_examples, n_threads, output);
  else 
    run_batch_compute(dbn, input, n_examples, n_threads, output);
  
  unprotect(1);
  return(output_r);
}


