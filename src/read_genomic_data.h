#ifndef read_genomic_data_H 
#define read_genomic_data_H

typedef struct {
  int n_sizes;
  int* window_sizes;
  int* half_n_windows;
  
  int* n_prev_bins;
} zoom_params_t;

typedef struct {
  double **forward;
  double **reverse;
} genomic_data_point_t;

typedef struct {
  int size;

  int *forward;
  int *reverse;
} raw_data_t;

#endif

