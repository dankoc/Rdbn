## read_genomic_data -- reads genomic data in the specified zoom parameters...
##

#' Reads genomic data from the specified position...
#'
#' @param bed A data.frame of genomic regions in bed format (chrom, start, end).
#' @param big_wig_plus Path to bigwig file representing GRO-seq/ PRO-seq reads on the plus strand.
#' @param big_wig_minus Path to bigwig file representing GRO-seq/ PRO-seq reads on the minus strand.
#' @param as_matrix If true, returns a matrix object.
#' @param window_sizes Size of windows in the model.
#' @param half_nWindows Number of windows on each side of the center position to evaluate.
#' @return Returns a list() object, where each element in the list is the zoom data
#' centered on a 
read_genomic_data <- function(bed, big_wig_plus, big_wig_minus, as_matrix= TRUE, window_sizes= c(10, 50, 500), half_nWindows= c(20, 20, 20)) {

  stopifnot(NROW(window_sizes) == NROW(half_nWindows))
  zoom<- list(as.integer(window_sizes), as.integer(half_nWindows))

  .Call("get_genomic_data_R", as.character(bed[,1]), as.integer((bed[,3]+bed[,2])/2), as.character(plus), as.character(minus), zoom, PACKAGE= "Rdbn")

  if(as_matrix) 
    dat <- t(matrix(unlist(dat), ncol=NROW(bed)))
    #dat <- t(matrix(unlist(lapply(c(1:NROW(dat)), function(x) {unlist(dat[[x]])})), ncol=NROW(bed)))

	return(dat)
}
