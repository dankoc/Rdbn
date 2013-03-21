#ifndef read_genomic_data_H 
#define read_genomic_data_H
#include "bigwiglib.h"

typedef struct {
  double **forward_data;
  double **reverse_data;
} genomic_data_t;

#endif

