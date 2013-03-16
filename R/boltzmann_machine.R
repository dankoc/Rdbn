## Boltzman Machine.
##

#` An S4 class that stores a basic boltzman machine.
#` @slot n_inputs The number of input nodes.
#` @slot n_outputs The number of output nodes.
#` @slot learning_rate The learning rate.
#` @slot io_weights A matrix of weights.
#` @export
setClass("rbm",#"restricted_boltzman_machine", 
  representation(
    n_inputs="integer",      ## Number of nodes in the input layer.
	n_outputs="integer",     ## Number of nodes in the output layer.
	learning_rate="numeric", ## How quickly the netowrk 'learns'.
	io_weights="matrix"      ## The actual weights matrix, connecting each input to output.
  )
)

# constructor.
# e.g.:
# rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
rbm <- function(n_inputs, n_outputs, learning_rate=0.1, io_weights=NULL) {
    if(is.null(io_weights)) {
	  io_weights <- matrix(rnorm(n_inputs*n_outputs, mean=0, sd=0.1), ncol=n_outputs)
	}
	else { ## If specified ... check dimenstions.
	  stopifnot(NROW(io_weights) == n_inputs & NCOL(io_weights) == n_outputs)
	}
    new("rbm", n_inputs=n_inputs, n_outputs=n_outputs, learning_rate=learning_rate, io_weights=io_weights)
  }

#` Method to train a boltzman machine (stored in rbm).
#` @param rbm The boltzman machine.
#` @param data A data matrix wherein each row represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("train", 
  def=function(rbm, data) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("train")
  })
  
setMethod("train", c(rbm="rbm"), 
  function(rbm, data) {
  	stopifnot(NCOL(data) == rbm@n_inputs)
	print("TRAINING!")
  })
  
#` 'Clamps' the input vector and runs the boltzmann machine to get the output node (i.e. get output conditional on specified input).
setGeneric("run_input", 
  def=function(rbm, set_input) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("run_input")
  })
  
setMethod("run_input", c(rbm="rbm"), 
  function(rbm, set_input) {
  	stopifnot(NROW(set_input) == rbm@n_inputs)
	print("Condition on an input!")
  })
  
    
#` 'Clamps' the output vector and runs the boltzmann machine to get the output node (i.e. get expected input conditional on the specified output).
setGeneric("run_output", 
  def=function(rbm, set_input) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("run_output")
  })
  
setMethod("run_output", c(rbm="rbm"), 
  function(rbm, set_output) {
  	stopifnot(NROW(set_input) == rbm@n_outputs)
	print("Condition on an output!")
  })

#     x %*% y
