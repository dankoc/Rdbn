## read_genomic_data -- reads genomic data in the specified zoom parameters...
##

#' Reads genomic data from the specified position...
#'
#' @param bed A data.frame of genomic regions in bed format (chrom, start, end).
#' @param big_wig_plus Bigwig file representing GRO-seq/ PRO-seq reads on the plus strand.
#' @param big_wig_minus Bigwig file representing GRO-seq/ PRO-seq reads on the minus strand.
#' @param as_matrix If true, returns a matrix object.
#' @param zoom List object describing the zoom.
#' @return Returns a list() object, where each element in the list is the zoom data
#' centered on a 
read_genomic_data <- function(bed, big_wig_plus, big_wig_minus, as_matrix= TRUE, rev_minus= TRUE,
		zoom= list(window_sizes= as.integer(c(10, 50, 500)), half_n_windows= as.integer(c(20, 20, 20)))) {

  max_size <- max(zoom$window_sizes*zoom$half_n_windows)
		
  dat <- lapply(unique(as.character(bed[,1])), function(x) {
   indx <- which(bed[,1] == x)
   center <- (bed[indx,3]+bed[indx,2])/2

   start_pos <- min(center)-max_size-1
   end_pos <- max(center)+max_size+2
   
   plus <- collect.counts(big_wig_plus,  chrom=x, start= start_pos, end= end_pos, step=1)
   minus<- collect.counts(big_wig_minus, chrom=x, start= start_pos, end= end_pos, step=1) 
   if(rev_minus) minus <- -1*minus ## Assume that minus strand is expressed in negative numbers.

   .Call("get_genomic_data_R", as.integer(center-start_pos+1), as.integer(plus), as.integer(minus), zoom, PACKAGE= "Rdbn")
  })

  if(as_matrix) 
    dat <- t(matrix(unlist(dat), ncol=NROW(bed)))
    #dat <- t(matrix(unlist(lapply(c(1:NROW(dat)), function(x) {unlist(dat[[x]])})), ncol=NROW(bed)))
  return(dat)
}
