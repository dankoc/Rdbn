
################################################################################
#' Returns the output of the logistic function on a data vector.
#'
#' @param vect_val Description of \code{vect_val}.  A numeric value, vector, or matrix to transform.
logistic_function <- function(vect_val) {
    return(1/(1+exp(-vect_val)))
}

