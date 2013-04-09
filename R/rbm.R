## Boltzman Machine.
##


#` An S4 class that stores a basic boltzman machine.
#` @slot n_inputs The number of input nodes.
#` @slot n_outputs The number of output nodes.
#` @slot learning_rate The learning rate.
#` @slot io_weights A matrix of weights.
#` @export
setClass("dbn_layer",#"restricted_boltzman_machine", 
  representation(
    n_inputs="integer",            ## Number of nodes in the input layer.
    n_outputs="integer",           ## Number of nodes in the output layer.
    io_weights="matrix",           ## The actual weights matrix, connecting each input to output.
#   bias_inputs="numeric",         ## Specified by bias_outputs[[n-1]]
    bias_outputs="numeric",        ## Bias vector, outputs.

    ## Learning parameters.  These might be updated as learning progresses.
    learning_rate="numeric",       ## How quickly the netowrk 'learns'.
	cd_n="integer",                ## Number of steps of Gibbs sampling when doing contrastive divergence.
	batch_size="integer",          ## Number of input examples to use in a 'mini-batch'.
#	update_input_biases="logical", ## Whether or not to update the biases of the input layer.
	
	## Special learning options.  These are NOT guaranteed to be set.
	## See in: http://www.cs.utoronto.ca/~ilya/pubs/ilya_sutskever_phd_thesis.pdf; pp. 75; also see: pp.5(background),73(Adapting Nesterov methods).
	use_momentum="logical",        ## Use momentum during fitting.
	momentum_decay="numeric",      ## \Mu; Rate at which old gradients are discarded.
	momentum="matrix"              ## Momentum term; serves as memory for other mini-batch members.  Speeds the rate of convergence.
  ),
)

dbn_layer <- function(n_inputs, 
                      n_outputs, 
                      batch_size=1, 
                      learning_rate=0.1, 
                      cd_n=1, 
                      momentum_decay= NA, 
                      io_weights=NULL, 
                      bias_outputs=NULL) 
{

  ## Initialize weights.
  if(is.null(io_weights)) {
    io_weights <- matrix(rnorm(n_inputs*n_outputs, mean=0, sd=0.01), ncol=n_outputs)
  }
  else { ## If specified ... check dimenstions.
    stopifnot(NROW(io_weights) == n_inputs & NCOL(io_weights) == n_outputs)
  }

  ## Initialize bias vectors for outputs.	
  if(is.null(bias_outputs)) {
    bias_outputs <- rep(0, n_outputs)#rnorm(n_outputs, mean=0, sd=0.01)
  }
  else {
    stopifnot(n_outputs == NROW(bias_outputs))
  }

  if(is.na(momentum_decay)) {
    use_momentum=FALSE
    momentum <- matrix(integer(0))
  }
  else {
    stopifnot(momentum_decay <= 1 & momentum_decay >= 0) ## Momentum decay between 0 and 1.
    use_momentum=TRUE
    momentum <- matrix(0, nrow=n_inputs, ncol=n_outputs)
  }

  new("dbn_layer", 
    n_inputs=as.integer(n_inputs), 
    n_outputs=as.integer(n_outputs), 
    cd_n=as.integer(cd_n), 
    batch_size=as.integer(batch_size),
    learning_rate=as.real(learning_rate), 
    io_weights=io_weights, 
    bias_outputs= bias_outputs,
    use_momentum= as.logical(use_momentum), 
    momentum= momentum,
    momentum_decay= as.real(momentum_decay))
}

#` An S4 class that stores a basic boltzman machine.
#` @slot n_inputs The number of input nodes.
#` @slot n_outputs The number of output nodes.
#` @slot learning_rate The learning rate.
#` @slot io_weights A matrix of weights.
#` @export
setClass("rbm",#"restricted_boltzman_machine", 
  contains="dbn_layer",
  representation(
    bias_inputs="numeric"          ## Bias vector, inputs.
  ),
)

# constructor.
# e.g.:
# rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
# Initial weights set using S8.1 in: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
rbm <- function(n_inputs, 
                n_outputs, 
                batch_size=1,
                learning_rate=0.1, 
                cd_n=1, 
                momentum_decay= NA, 
                io_weights=NULL, 
                bias_inputs=NULL, 
                bias_outputs=NULL) 
{

  ## Initialize bias vectors for inputs.
  if(is.null(bias_inputs)) {
    bias_inputs <- rep(0, n_inputs)#rnorm(n_inputs, mean=0, sd=0.01)
  }
  else {
    stopifnot(n_inputs == NROW(bias_inputs))
  }

  new("rbm", 
    dbn_layer(
      n_inputs=as.integer(n_inputs), 
      n_outputs=as.integer(n_outputs), 
      cd_n=as.integer(cd_n), 
      batch_size=as.integer(batch_size),
      learning_rate=as.real(learning_rate), 
      io_weights=io_weights, 
      bias_outputs= bias_outputs,
      use_momentum= as.logical(use_momentum), 
      momentum= momentum,
      momentum_decay= as.real(momentum_decay))
    bias_inputs= as.real(bias_inputs))
}

 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to train a boltzman machine (stored in rbm).
#` @param rbm The boltzman machine.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("rbm.train", 
  def=function(rbm, data, n_epocs= 1000) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("rbm.train")
})
  
setMethod("rbm.train", c(rbm="rbm"), 
  function(rbm, data, n_epocs= 1000) { ## Regularization/ LASSO type options?!
  	stopifnot(NROW(data) == rbm@n_inputs)
	
	## Reassign input biases to training example requencies?!  As suggested in: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
	rbm@bias_inputs <- as.real(rowSums(data)/NCOL(data))
	
	## Pass to C for training.
    .Call("train_rbm_R", rbm, as.real(data), as.integer(n_epocs), package="Rdbn") 
})

###########################################################################################################################################
### Brought these back for now. Perhaps useful?! ##########################################################################################
  
#` 'Clamps' the input vector and runs the boltzmann machine to get the output node (i.e. get output conditional on specified input).
setGeneric("clamp_input", 
  def=function(rbm, set_input, ...) {
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
  def=function(rbm, set_output, ...) { 
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
sample_states <- function(vect_pr) {
    vect_pr > runif(NROW(vect_pr))
}#)

## Returns the output of the logicstic function.
#setGeneric("logistic_function", def=function(rbm, vect_val) {standardGeneric("logistic_function")} )
#setMethod("logistic_function", c(rbm="rbm"), 
logistic_function <- function(vect_val) {
    return(1/(1+exp(-vect_val)))
}#)


#     x %*% y
