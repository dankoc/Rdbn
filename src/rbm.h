#ifndef restricted_boltzmann_machine_H 
#define restricted_boltzmann_machine_H
#include "matrix_functions.h"

typedef struct {
  int n_inputs;         // Number of nodes in the lower 'input' layer.
  int n_outputs;        // Number of nodes in the upper 'output' layer.
  
  matrix_t *io_weights; // n_inputs x n_outputs matrix representing weights between input and output nodes.
  double *bias_inputs;  // 1x n_inputs vector representing the 'bias' toward activation of the input nodes.
  double *bias_outputs; // 1x n_outputs vector representing the 'bias' toward activation of the input nodes.

  double learning_rate; // Rate at which the model learns.
  int cd_n;             // Specifies the number of Gibbs sampling steps used for contrastive divergence during training.
  int batch_size;       // Specifies the batch size for training.
} rbm_t;

rbm_t *alloc_rbm(int n_inputs, int n_outputs);
void free_rbm(rbm_t *rbm);
void init_rbm(rbm_t *rbm, double learning_rate, int batch_size, int cd_n, double expected_frequency_on);

void clamp_input(rbm_t *rbm, double *input, double *resulting_output);
void clamp_output(rbm_t *rbm, double *output, double *resulting_input);

rbm_t *rbm_r_to_c(SEXP rbm_r);
void train(rbm_t *rbm, double *input_example);

#endif

