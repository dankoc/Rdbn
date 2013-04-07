#ifndef restricted_boltzmann_machine_H 
#define restricted_boltzmann_machine_H
#include "matrix_functions.h"

typedef struct {
  // Basic description of a restricted Boltzman machine.
  int n_inputs;           // Number of nodes in the lower 'input' layer.
  int n_outputs;          // Number of nodes in the upper 'output' layer.
  
  matrix_t *io_weights;   // n_inputs x n_outputs matrix representing weights between input and output nodes.
  double *bias_inputs;    // 1x n_inputs vector representing the 'bias' toward activation of the input nodes.
  double *bias_outputs;   // 1x n_outputs vector representing the 'bias' toward activation of the input nodes.

  // Basic learning parameters.
  double learning_rate;   // Rate at which the model learns.
  int cd_n;               // Specifies the number of Gibbs sampling steps used for contrastive divergence during training.
  int batch_size;         // Specifies the batch size for training.
  
  // Special learning options.  These are NOT guaranteed to be set.
  // See in: http://www.cs.utoronto.ca/~ilya/pubs/ilya_sutskever_phd_thesis.pdf; pp. 75; also see: pp.5(background),73(Adapting Nesterov methods).
  int use_momentum;       // Use momentum during fitting.
  double momentum_decay;  // \Mu; Rate at which old gradients are discarded.
  matrix_t *momentum;     // Momentum term; serves as memory for other mini-batch members.  Speeds the rate of convergence.
  
  // Also implement sparsity controls(?!).
} rbm_t;

typedef struct {
  matrix_t *delta_w;
  double *delta_output_bias;
  double *delta_input_bias;
} delta_w_t;

rbm_t alloc_rbm(int n_inputs, int n_outputs);
void free_rbm(rbm_t rbm);
rbm_t init_rbm(rbm_t rbm, double learning_rate, int batch_size, int cd_n, double expected_frequency_on);

void clamp_input(rbm_t rbm, double *input, double *resulting_output);
void clamp_output(rbm_t rbm, double *output, double *resulting_input);

rbm_t rbm_r_to_c(SEXP rbm_r);
void rbm_train(rbm_t rbm, double *input_example, int n_examples, int n_epocs);

SEXP train_rbm_R(SEXP rbm_r, SEXP training_data_r, SEXP n_epocs_r);

#endif

