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
    for(int j=0;j<(2*zoom.half_n_windows);j++) {
      dp.forward[i][j]= 0;
      dp.reverse[i][j]= 0;
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
  for(int i=0;<n_sizes;i++) {
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
  int max_size= window_size*half_n_windows;
  if(position < (center-max_size) || position > (center+max_size) || position == center) return(-1);
  int dist_from_start= (position<center)?(position-center):(position-center-1); // Because center position isn't included ... have to subtract 1 for windows right of center.
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
  init_genomic_data_point(dp);

  // Get the max boundary of our window.
  int max_bounds = max_dist_from_center(zoom.n_sizes, zoom.window_sizes, zoom.half_n_windows);
  int left_edge= center - max_bounds;
  int right_edge= center + max_bounds + 1;
  
  // Loop through incrementing each vector.
  for(int bp= left_edge;bp<= right_edge;bp++) {
    for(int i=0;i<n_sizes;i++) {
	  int which_bin= get_bin_number(center, bp, zoom.window_sizes[i], zoom.half_n_windows[i]);
      if(which_bin>0) {
	    dp.forward_data[i][which_bin]+= chrom_counts.forward[bp];
		dp.reverse_data[i][which_bin]+= chrom_counts.reverse[bp];
	  }
    }
  }

}

/*
 * Scales genomic data ... following a logistic function with the specified parameters...
 *  Pass this information in zoom_params_t(?!).  Or store it in genomic_data_point_t?  Or separate struct?!
 */
//void scale_genomic_data(zoom_params_t zoom, genomic_data_point_t dp) {}

/*
 * R entry point ... for getting a particular center (or vector of centers).
 */
SEXP get_genomic_data_R(SEXP centers, SEXP plus_counts, SEXP minus_counts, SEXP model) {
  int n_centers = Rf_nrows(centers);
  int *centers = INTEGER(centers);
  
  // Set up model variable.
  
  // Set up raw data to work with C.
  raw_data_t rd;
  rd.forward= INTEGER(plus_counts);
  rd.reverse= INTEGER(minus_counts);
  
  // Set up return variable.
  SEXP processed_data;
  
  for(int i=0;i<n_centers;i++) {
    max_dist_from_center(zoom.n_sizes, zoom.window_sizes, zoom.half_n_windows);
    if() ;
  }
  
  return();
}

/*
 * We need a simple vector to pass into the more general (??) functions.
 */
int *genomic_data_point_to_vector() {
  
  int *c_list = (int*)calloc((2*zoom.n_prev_bins[zoom.n_sizes]), sizeof(int)); // Because I'm going to destroy it ...

}