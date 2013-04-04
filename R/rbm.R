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
    io_weights="matrix",      ## The actual weights matrix, connecting each input to output.
    bias_inputs="numeric",    ## Bias vector, inputs.
    bias_outputs="numeric"   ## Bias vector, outputs.
  ),
)

# constructor.
# e.g.:
# rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
# Initial weights set using S8.1 in: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
rbm <- function(n_inputs, n_outputs, learning_rate=0.1, io_weights=NULL, bias_inputs=NULL, bias_outputs=NULL) {
    ## Initialize weights.
    if(is.null(io_weights)) {
	  io_weights <- matrix(rnorm(n_inputs*n_outputs, mean=0, sd=0.01), ncol=n_outputs)
	}
	else { ## If specified ... check dimenstions.
	  stopifnot(NROW(io_weights) == n_inputs & NCOL(io_weights) == n_outputs)
	}

	## Initialize bias vectors for inputs.
	if(is.null(bias_inputs)) {
	  bias_inputs <- rep(0, n_inputs)#rnorm(n_inputs, mean=0, sd=0.01)
	}
	else {
	  stopifnot(n_inputs == NROW(bias_inputs))
	}

	## Initialize bias vectors for outputs.	
	if(is.null(bias_outputs)) {
	  bias_outputs <- rep(0, n_outputs)#rnorm(n_outputs, mean=0, sd=0.01)
	}
	else {
	  stopifnot(n_outputs == NROW(bias_outputs))
	}

    new("rbm", n_inputs=n_inputs, n_outputs=n_outputs, learning_rate=learning_rate, 
			io_weights=io_weights, bias_inputs= bias_inputs, bias_outputs= bias_outputs)
}

 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to train a boltzman machine (stored in rbm).
#` @param rbm The boltzman machine.
#` @param data A data matrix wherein each row represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("train", 
  def=function(rbm, data, ...) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("train")
})
  
setMethod("train", c(rbm="rbm"), 
  function(rbm, data, batch_size=10, cdn= 1) { ## Regularization/ LASSO type options?!
  	stopifnot(NROW(data) == rbm@n_inputs)
    .Call("train_rbm_R", rbm, as.real(data), as.integer(batch_size), as.integer(cdn), package="Rdbn") 
})

