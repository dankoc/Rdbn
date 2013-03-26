## read_genomic_data -- reads genomic data in the specified zoom parameters...
##

#' Reads genomic data from the specified position...
#'
#' @param center Vector of positions on which to collect data.
#' @param plus Vector of counts on plus strand (1 bp bins).
#' @param minus Vector of counts on minus strand (1 bp bins).
#' @param as_matrix If true, returns a matrix object.
#' @param zoom List object describing the zoom.
#' @return Returns a list() object, where each element in the list is the zoom data
#' centered on a 
read_genomic_data <- function(center, plus, minus, as_matrix= TRUE, 
		zoom= list(window_sizes= c(50, 500, 5000), half_n_windows= c(20, 20, 20))) {
  
  dat <- .Call("get_genomic_data_R", as.integer(center), as.integer(plus), as.integer(minus), zoom, PACKAGE= "Rdbn")
  if(!as_matrx) return(dat)

  dat_l <- lapply(c(1:NROW(dat)), functin(x) {unlist(dat[[x]])})
  return(dat_l)
}