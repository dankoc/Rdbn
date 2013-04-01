#ifndef restricted_boltzmann_machine_H 
#define restricted_boltzmann_machine_H

typedef struct {
  int n_inputs;         // Number of nodes in the lower 'input' layer.
  int n_outputs;        // Number of nodes in the upper 'output' layer.
  
  double learning_rate; // Rate at which the model learns.
  double **io_weights;  // n_inputs x n_outputs matrix representing weights between input and output nodes.
  double *bias_inputs;  // 1x n_inputs vector representing the 'bias' toward activation of the input nodes.
  double *bias_outputs; // 1x n_outputs vector representing the 'bias' toward activation of the input nodes.
} rbm_t;

#endif

