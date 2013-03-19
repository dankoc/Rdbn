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
	bias_inputs="numeric"    ## Bias vector, inputs.
	bias_outputs="numeric"   ## Bias vector, outputs.
  )
)

# constructor.
# e.g.:
# rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
rbm <- function(n_inputs, n_outputs, learning_rate=0.1, io_weights=NULL, bias_inputs=NULL, bias_outputs=NULL) {
    ## Initialize weights.
    if(is.null(io_weights)) {
	  io_weights <- matrix(rnorm(n_inputs*n_outputs, mean=0, sd=0.1), ncol=n_outputs)
	}
	else { ## If specified ... check dimenstions.
	  stopifnot(NROW(io_weights) == n_inputs & NCOL(io_weights) == n_outputs)
	}

	## Initialize bias vectors for inputs.
	if(is.null(bias_inputs)) {
	  bias_inputs <- rnorm(n_inputs, mean=0, sd=0.1)
	}
	else {
	  stopifnot(n_inputs == NROW(bias_inputs))
	}

	## Initialize bias vectors for outputs.	
	if(is.null(bias_outputs)) {
	  bias_outputs <- rnorm(n_outputs, mean=0, sd=0.1)
	}
	else {
	  stopifnot(n_outputs == NROW(bias_outputs))
	}

    new("rbm", n_inputs=n_inputs, n_outputs=n_outputs, learning_rate=learning_rate, 
			io_weights=io_weights, bias_inputs= bias_inputs, bias_outputs= bias_outputs)
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
  function(rbm, data, cdn= 1, batch_size=10) { ## Regularization/ LASSO type options?!
  	stopifnot(NCOL(data) == rbm@n_inputs)
	for(i in 1:NROW(data)) {
	  ## Compute p(hj=1 | v)= logistic_sigmoid(b_j+\sum(v_i * w_ij)); logistic_sigmoid= (1/(1+exp(-x)))
	   # Sample on/off for this hidden unit; generate unif. rand 0:1.  On in greater.
	  ## Just run clamp_input function to the training data.
	  out_samp_vec <- clamp_input(rbm, data[i,])
	  
	  ## Compute <vihj>_data
	   # 
	  in_samp_vec <- clamp_output(rbm, out_samp_vec)
	  out2_samp_vec <- clamp_output(rbm, in_samp_vec)
	  
	  ## Compute <vihj>_recon
	  c_data <- in_samp_vec*
	  
	  ## Update weights.
	  rmb@io_weights <- rmb@io_weights+rmb@learning_rate*(c_data-c_recon)
	}
	print("TRAINING!")
  })
  
#` 'Clamps' the input vector and runs the boltzmann machine to get the output node (i.e. get output conditional on specified input).
setGeneric("clamp_input", 
  def=function(rbm, set_input) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("clamp_input")
  })
  
setMethod("clamp_input", c(rbm="rbm"), 
  function(rbm, set_input, return_prob= FALSE) {
  	stopifnot(NROW(set_input) == rbm@n_inputs)
	                                                      ## Or is it transpose of this?!
	pr_act <- logistic_function(rbm@bias_outputs + set_input %*% rbm@io_weights) 
	
	if(return_prob) return(pr_act)
	else return(sample_states(pr_act))
  })
  
    
#` 'Clamps' the output vector and runs the boltzmann machine to get the output node (i.e. get expected input conditional on the specified output).
setGeneric("clamp_output", 
  def=function(rbm, set_output) { 
	stopifnot(class(rbm) == "rbm")
	standardGeneric("clamp_output")
  })
  
## Returns a prob. by default, as suggested in S3.2 of guideTR.pdf.
setMethod("clamp_output", c(rbm="rbm"), 
  function(rbm, set_output, return_prob= TRUE) {
  	stopifnot(NROW(set_output) == rbm@n_outputs)
	                                                   ## Is this even close to right?!
	pr_act <- logistic_function(rbm@bias_inputs + set_output %*% rbm@io_weights) 
	
	if(return_prob) return(pr_act)
	else return(sample_states(pr_act))
  })

##################################################################
## No reason to make these descend directly from the rbm class...

## Returns a vector of probabilities by sampling the input vect_pr.
#setGeneric("sample_states", def=function(rbm, vect_pr) {standardGeneric("sample_states")} )
#setMethod("sample_states", c(rbm="rbm"), 
sample_states <- function(rbm, vect_pr) {
    vect_pr > runif(NROW(vect_pr))
}#)

## Returns the output of the logicstic function.
#setGeneric("logistic_function", def=function(rbm, vect_val) {standardGeneric("logistic_function")} )
#setMethod("logistic_function", c(rbm="rbm"), 
logistic_function <- function(rbm, vect_val) {
    return(1/(1+exp(-vect_val)))
}#)


#     x %*% y
