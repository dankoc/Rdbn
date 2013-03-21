#ifndef read_genomic_data_H 
#define read_genomic_data_H
#include "bigwiglib.h"

typedef struct {
  int n_sizes;
  int* window_sizes;
  int* half_n_windows;
} zoom_params_t;

typedef struct {
  double **forward_data;
  double **reverse_data;
} genomic_data_point_t;

typedef struct {
  int *forward;
  int *reverse;
} raw_data_t;

#endif

