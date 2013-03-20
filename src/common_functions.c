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

/*
 * Returns log(p^X)= X*log(p)
 */
double l_power(double p, int X) {
  return(log(p)*(double)X);
}



/*
 * expSum -- Robustly calculates log(exp(log(x))+exp(log(y))).
 */
double expSum(double logx, double logy) {
  assert(!isnan(logx) && !isnan(logy));
  // expSum w/ just two values.  Inelegant, yet saves having to copy two doubles to a new double*.
  double scalefactor= logx>logy?logx:logy;
  double robustExpSum= log(exp(logx-scalefactor)+exp(logy-scalefactor))+scalefactor;
  return(robustExpSum);
}

/*
 * expSum -- Robustly calculates log(exp(log(x))+exp(log(y))+exp(log(z))).
 */
double expSum_3(double logx, double logy, double logz) {
  double scalefactor= logx>logy?logx:logy;
  scalefactor= scalefactor>logz?scalefactor:logz;
  double robustExpSum= log(exp(logx-scalefactor)+exp(logy-scalefactor)+exp(logz-scalefactor))+scalefactor;
  return(robustExpSum);
}


/*
 * expDif -- Robustly returns log(abs(exp(pGr)-exp(pLs))).
 */
double expDif(double pLs, double pGr) {
  assert(!isnan(pLs) && !isnan(pGr));
  if(pGr == pLs) 
    return(log(0));
  else if(pGr > pLs)
	return(log(1-exp(pLs-pGr))+pGr);
  else
    return(log(1-exp(pGr-pLs))+pLs);
}

/* 
 * Does the necessary averaging inside of a window. 
 *
 * Arguments:
 *  left_edge     --> Left edge to evaluate.
 *  n_windows  --> Number of independent windows in the model =(2*half_n+1).
 *  windowSize --> Full window size =(2*half_size+1).
 *  data       --> Bigwig file object.
 *
 * Definitions and reasoning in notebook entry from 2/8/13
 */
int *get_windowSums_i(int left_edge, int n_windows, int windowSize, int* data) {
  // Get number of counts in each window (allowing overlap)
  int *windowSums = (int*) calloc(n_windows, sizeof(int));
  for(int i=0;i<n_windows;i++) {
    windowSums[i] = 0;
	for(int b=0;b<windowSize;b++) {
	  windowSums[i] += data[left_edge+b+i*windowSize];
	}
  }
  return(windowSums);
}

/*
 *  Turns around a (short) vector.
 *  Leaves the origional untouched.
 */
int *reverse_vector(int *vector, int n_elements) {
  int *rev_vector= (int*) calloc(n_elements, sizeof(int));
  for(int i=0;i<n_elements;i++) {
    rev_vector[i] = vector[n_elements-i-1];
  }
  return(rev_vector);
}

/*
 * Returns maximum bounds around a center position.
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
 * Returns the bin that a particular posotion falls into,
 * or -1 for 'outside' ...
 *
 */
int get_bin_number(int center, int window_size, int half_n_windows) {
  bin_number=-1;
  // How to get the bin number with these variables?!
}

/*
 * Returns vector of counts for each windows in genomic data.
 *
 * IDEA: Perhaps have a variant of this function that just returns the linear vector?!  
 * This would be useful for training all inside C.
 *
 * To do this efficently, loop through the region once.  
 *   Assume ... (1) user dosen't pass in something beyond the bounds of chrom_counts.
 *
 * Arguments: 
 *  center  --> Center position 
 *  n_sizes --> Number of 
 */
int *get_genomic_data(int center, int n_sizes, int* window_sizes, int* half_n_windows, int* chrom_counts_plus, int* chrom_counts_minus) {
  int *c_list = calloc( ... ); // Because I'm going to destroy it ...
  
  // Init. get offset for each n_sizes in c_list.  
  int *n_prev_bins = (int)R_alloc(n_sizes, sizeof(int*));
  n_prev_bins[0]= 0;
  for(int i=1;i<n_sizes;i++) {
    n_prev_bins[i] = n_prev_bins[i-1]+2*half_n_windows[i-1]; 
  }
  
  // Get the max boundary of our window.
  int max_bounds = max_dist_from_center(n_sizes, window_sizes, half_n_windows);
  int left_edge= center - max_bounds; // Should there be a +/- 1 on any of these?
  int right_edge= center + max_bounds;
  
  // Loop through incrementing each vector.
  for(int bp= left_edge;bp<= right_edge;bp++) {
    for(int i=0;i<n_sizes;i++) {
	  int which_bin= get_bin_number(center, window_sizes[i], half_n_windows[i]);
      if(which_bin>0) {
	    c_list.plus[n_prev_bins[i]+which_bin]+= chrom_counts_plus[bp];
		c_list.minus[(n_prev_bins[n_sizes-1]+)+n_prev_bins[i]+which_bin]+= chrom_counts_minus[bp]; // This is entire plus strand, plus current position.
	  }
    }
  }
  
  return(c_list);
}

