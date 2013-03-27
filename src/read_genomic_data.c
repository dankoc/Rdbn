/*
 * Functions required to work with logs.  
 *
 */
#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Applic.h>
#include <assert.h>
#include "read_genomic_data.h"

/*
 * set_zoom_params --> Creates a new zoom_params_t data type.
 */
zoom_params_t set_zoom_params(int n_sizes, int* window_sizes, int* half_n_windows) {
  zoom_params_t zoom;
  zoom.window_sizes = window_sizes;
  zoom.half_n_windows = half_n_windows;
  
  // This is JUST for translating understandable object into a nice vector.
  zoom.n_prev_bins = (int*)R_alloc(n_sizes+1, sizeof(int));
  zoom.n_prev_bins[0]= 0;
  for(int i=1;i<n_sizes;i++) {
    zoom.n_prev_bins[i] = zoom.n_prev_bins[i-1]+2*half_n_windows[i-1]; 
  }
  zoom.n_prev_bins[n_sizes] = zoom.n_prev_bins[n_sizes-1]+2*half_n_windows[n_sizes-1];
  
  return(zoom);
}

/*
 * init_genomic_data_point --> Initializes genomic_data_point_t to 0.
 */
void init_genomic_data_point(genomic_data_point_t dp, zoom_params_t zoom) {
  for(int i=0;i<zoom.n_sizes;i++) {
    for(int j=0;j<(2*zoom.half_n_windows[i]);j++) {
      dp.forward[i][j]= 0.0f;
      dp.reverse[i][j]= 0.0f;
    }
  }
}

/*
 * alloc_genomic_data_point --> Allocates a new genomic_data_point_t.
 */
genomic_data_point_t alloc_genomic_data_point(zoom_params_t zoom) {
  genomic_data_point_t dp;
  dp.forward = (double**)calloc(zoom.n_sizes,sizeof(double*));
  dp.reverse = (double**)calloc(zoom.n_sizes,sizeof(double*));
  
  for(int i=0;i<zoom.n_sizes;i++) {
    dp.forward[i] = (double*)calloc((2*zoom.half_n_windows[i]),sizeof(double));
    dp.reverse[i] = (double*)calloc((2*zoom.half_n_windows[i]),sizeof(double));
  }
  
  return(dp);
}

/*
 * alloc_genomic_data_point --> creates a new genomic_data_point_t.
 */
void free_genomic_data_point(genomic_data_point_t dp, zoom_params_t zoom) {
  for(int i=0;i<zoom.n_sizes;i++) {
    free(dp.forward[i]);
    free(dp.reverse[i]);
  }

  free(dp.forward);
  free(dp.reverse);
}

/*
 * max_dist_from_center --> Returns maximum bounds around a center position.
 */
int max_dist_from_center(int n_sizes, int *window_sizes, int *half_n_windows) {
  int max_bounds=0;
  for(int i=0;i<n_sizes;i++) {
    int curr_bounds= window_sizes[i]*half_n_windows[i];
	if(max_bounds < curr_bounds) max_bounds= curr_bounds;
  }
  return(max_bounds);
}

/*
 * get_bin_number --> Returns the bin that a particular posotion falls into.
 *
 * Note than since the center base is not included, it needs to be subtracted
 * from the position of interest, if that position falls past the center.
 */
int get_bin_number(int center, int position, int window_size, int half_n_windows) {
  // How to get the bin number with these variables?!
  int left= center-window_size*half_n_windows;
  int right= center+window_size*half_n_windows;
  if(position < left || position > right || position == center) return(-1);
  int dist_from_start= (position<center)?(position-left):(position-left-1); // Because center position isn't included ... have to subtract 1 for windows right of center.
  return((int)floor(((double)dist_from_start)/((double)window_size)));
}

/*
 * Returns vector of counts for each windows in genomic data.
 *
 * To do this efficently, loop through the region once.  
 *   Assume ... (1) user dosen't pass in something beyond the bounds of chrom_counts.
 *
 * Arguments: 
 *  center  --> Center position 
 *  n_sizes --> Number of 
 */
void get_genomic_data(int center, zoom_params_t zoom, raw_data_t chrom_counts, genomic_data_point_t dp) {
  init_genomic_data_point(dp, zoom);

  // Get the max boundary of our window.
  int max_bounds = max_dist_from_center(zoom.n_sizes, zoom.window_sizes, zoom.half_n_windows);
  int left_edge= center - max_bounds;
  int right_edge= center + max_bounds+1;
  
  // Sets up a bit of a  strange boundary condition, where 0's are included on all out-of-bounds windows.
  // After last night's experiment, I see this as preferable to throwing an error, however.
  if(right_edge >= chrom_counts.size) right_edge = chrom_counts.size;
  
  // Loop through incrementing each vector.
  for(int bp= left_edge;bp<right_edge;bp++) {
    for(int i=0;i<zoom.n_sizes;i++) {
      int which_bin= get_bin_number(center, bp, zoom.window_sizes[i], zoom.half_n_windows[i]);
      if(which_bin>=0) {
        dp.forward[i][which_bin]+= (double)chrom_counts.forward[bp];
        dp.reverse[i][which_bin]+= (double)chrom_counts.reverse[bp];
      }
    }
  }

}

/*
 * Returns the max value in data1 and data2.
 */
double get_max(int n, double* data1, double* data2) {
  double max=-1;
  for (int i=0;i<n;i++) {
    if(data1[i] > max) max= data1[i];
	if(data2[i] > max) max= data2[i];
  }
  return(max);
}

/*
 * Scales genomic data ... following a logistic function with the specified parameters...
 *  Pass this information in zoom_params_t(?!).  Or store it in genomic_data_point_t?  Or separate struct?!
 *
 * Scales using logistic function --> F(t)= 1/(1+e^(-\alpha(t-\beta)))
 *      Right now, \beta= MAX/2 (defines position of 0.5).
 *                 \alpha= 2*log(1/0.01 - 1)/MAX  (Signal at 0 reads set to 0.01). 
 */
void scale_genomic_data(zoom_params_t zoom, genomic_data_point_t dp) {
  double val_at_min=0.01;
  for(int i=0;i<zoom.n_sizes;i++) {
    // Get parameters.  Require value of 0.99 at MAX and 0.01 at 0.
    double max_val= get_max(2*zoom.half_n_windows[i], dp.forward[i], dp.reverse[i]);
    double alpha= 2*log(1/val_at_min - 1) / max_val;

	// Scale values with the logistic function.
    for(int j=0;j<2*zoom.half_n_windows[i]; j++) {
      dp.forward[i][j] = 1/ (1+ exp(-1*alpha*(dp.forward[i][j]-(max_val/2))));
      dp.reverse[i][j] = 1/ (1+ exp(-1*alpha*(dp.reverse[i][j]-(max_val/2))));
    }
  }
}

/*
 * Moves C genomic_data_point_t type to a SEXP for return to R.
 */
SEXP data_point_to_list(zoom_params_t zoom, genomic_data_point_t dp) {
  SEXP data_point;
  protect(data_point = allocVector(VECSXP, 2*zoom.n_sizes));
  
  for(int i=0;i<zoom.n_sizes;i++) {
    // Creat R object.
    SEXP size_t_for, size_t_rev;
    protect(size_t_for = allocVector(REALSXP, zoom.half_n_windows[i]*2));
    protect(size_t_rev = allocVector(REALSXP, zoom.half_n_windows[i]*2));
    SET_VECTOR_ELT(data_point, 2*i, size_t_for);
    SET_VECTOR_ELT(data_point, 2*i+1, size_t_rev);

    // Copy data from dp to R object.
    double *size_t_for_c = REAL(size_t_for);
    double *size_t_rev_c = REAL(size_t_rev);
    for(int j=0;j<2*zoom.half_n_windows[i];j++) {
      size_t_for_c[j] = dp.forward[i][j];
      size_t_rev_c[j] = dp.reverse[i][j];
    }
	UNPROTECT(2);
  }
  UNPROTECT(1);
  return(data_point);
}

/*
 * Moves C genomic_data_point_t type to a SEXP for return to R.
 * This function generates a single vector.
 */
SEXP data_point_to_vect(zoom_params_t zoom, genomic_data_point_t dp) {
  SEXP data_point;
  
  // Count  number of windows to allocate R vector... 
  int n_windows=0;
  for(int i=0;i<zoom.n_sizes;i++)
    n_windows+= zoom.half_n_windows[i]*2;
  
  protect(data_point = allocVector(REALSXP, 2*n_windows));
  double *data_point_c = REAL(data_point);
  
  int k=0;
  for(int i=0;i<zoom.n_sizes;i++) {
    for(int j=0;j<2*zoom.half_n_windows[i];j++) {
      data_point_c[k] = dp.forward[i][j];
      data_point_c[n_windows+k++] = dp.reverse[i][j];
    }
  }
  UNPROTECT(1);
  return(data_point);
}

/*
 * R entry point ... for getting a particular center (or vector of centers).
 *
 * Switch to R vector using:
 * t(matrix(unlist(list(c(1:10), c(11:20), c(0:9))), ncol=3))
 */
SEXP get_genomic_data_R(SEXP centers_r, SEXP plus_counts_r, SEXP minus_counts_r, SEXP model_r) {
  int n_centers = Rf_nrows(centers_r);
  int *centers = INTEGER(centers_r);
  
  // Set up model variable.
  zoom_params_t zoom;
  zoom.n_sizes= Rf_nrows(VECTOR_ELT(model_r, 0));
  zoom.window_sizes= INTEGER(VECTOR_ELT(model_r, 0));
  zoom.half_n_windows= INTEGER(VECTOR_ELT(model_r, 1));
  
  // Set up raw data to work with C.
  int n_positions= Rf_nrows(plus_counts_r);
  raw_data_t rd;
  rd.size= Rf_nrows(plus_counts_r);
  rd.forward= INTEGER(plus_counts_r);
  rd.reverse= INTEGER(minus_counts_r);
  
  // Set up return variable.
  genomic_data_point_t dp= alloc_genomic_data_point(zoom);
  SEXP processed_data;
  PROTECT(processed_data = allocVector(VECSXP, n_centers));

  for(int i=0;i<n_centers;i++) {
    int max_dist= max_dist_from_center(zoom.n_sizes, zoom.window_sizes, zoom.half_n_windows);
    get_genomic_data(centers[i], zoom, rd, dp); // Get data..
    scale_genomic_data(zoom, dp); // Scale data?!
    
    // Record ...
    SEXP data_point= data_point_to_vect(zoom, dp);//data_point_to_list(zoom, dp);
    SET_VECTOR_ELT(processed_data, i, data_point);
  }
  free_genomic_data_point(dp, zoom);
  UNPROTECT(1);//+n_centers);
  
  return(processed_data);
}

/*
 * We need a simple vector to pass into the more general (??) functions.
 */
/*int *genomic_data_point_to_vector() {
  
  int *c_list = (int*)calloc((2*zoom.n_prev_bins[zoom.n_sizes]), sizeof(int)); // Because I'm going to destroy it ...

}*/
