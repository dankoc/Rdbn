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
    bias_outputs="numeric",        ## Bias vector, outputs.

    ## Learning parameters.  These might be updated as learning progresses.
    learning_rate="numeric",       ## How quickly the netowrk 'learns'.
	cd_n="integer",                ## Number of steps of Gibbs sampling when doing contrastive divergence.
	batch_size="integer",          ## Number of input examples to use in a 'mini-batch'.
		
	## Special learning options.  These are NOT guaranteed to be set.

	## Using L2 regularization, a.k.a weight-decay.
    ## Note that this is ONLY applied to weights, NOT to biases...  per: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
    use_l2_penalty="logical",      ## Whether (or not) to apply L2 regularization.
	weight_cost="numeric",         ## Numeric.  If used, set between 1e-2 and 1e-5.
	
    ## 'Momentum method' for updating weights.
	## See in: http://www.cs.utoronto.ca/~ilya/pubs/ilya_sutskever_phd_thesis.pdf; pp. 75; also see: pp.5(background),73(Adapting Nesterov methods).
	use_momentum="logical",        ## Use momentum during fitting.
	momentum_decay="numeric"       ## \Mu; Rate at which old gradients are discarded.
  ),
)

dbn_layer <- function(n_inputs, 
                      n_outputs, 
                      learning_rate=0.1, 
                      batch_size=1, 
                      momentum_decay= NA, 
                      weight_cost= NA,					  
                      cd_n=1, 
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
    bias_outputs <- rep(0, n_outputs)
  }
  else {
    stopifnot(n_outputs == NROW(bias_outputs))
  }

  if(is.na(momentum_decay)) {
    use_momentum=FALSE
  }
  else {
    stopifnot(momentum_decay <= 1 & momentum_decay >= 0) ## Momentum decay between 0 and 1.
    use_momentum=TRUE
  }
  
  if(is.na(weight_cost)) {
    use_l2_penalty=FALSE
  }
  else {
    use_l2_penalty=TRUE
  }

  new("dbn_layer", 
    n_inputs=as.integer(n_inputs), 
    n_outputs=as.integer(n_outputs), 
    io_weights=io_weights, 
    bias_outputs= bias_outputs,
    learning_rate=as.real(learning_rate), 
    cd_n=as.integer(cd_n), 
    batch_size=as.integer(batch_size),
	use_l2_penalty=as.logical(use_l2_penalty),
	weight_cost=as.real(weight_cost),
    use_momentum= as.logical(use_momentum), 
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
                x= NULL,				
                batch_size=1,
                learning_rate=0.1, 
                cd_n=1, 
                momentum_decay= NA, 
                weight_cost= NA,
                io_weights=NULL, 
                bias_inputs=NULL, 
                bias_outputs=NULL, ...) 
{

  ## Initialize bias vectors for inputs.
  if(is.null(bias_inputs)) {
    bias_inputs <- rep(0, n_inputs)
  }
  else {
    stopifnot(n_inputs == NROW(bias_inputs))
  }

  rbm <- new("rbm", 
    dbn_layer(
      n_inputs=as.integer(n_inputs), 
      n_outputs=as.integer(n_outputs), 
      io_weights=io_weights, 
      bias_outputs= bias_outputs,
      weight_cost= weight_cost,
      learning_rate=as.real(learning_rate), 
      cd_n=as.integer(cd_n), 
      batch_size=as.integer(batch_size),
      momentum_decay= as.real(momentum_decay)),
    bias_inputs= as.real(bias_inputs))
  
  if(!is.null(x)) {
    rbm <- rbm.train(rbm, data= x, ...)
  }
  
  return(rbm)
}

 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to train a boltzman machine (stored in rbm).
#` @param rbm The boltzman machine.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("rbm.train", 
  def=function(rbm, data, n_epocs= 1000, n_threads=1) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("rbm.train")
})
  
setMethod("rbm.train", c(rbm="rbm"), 
  function(rbm, data, n_epocs= 1000, n_threads=1) { ## Regularization/ LASSO type options?!
  	stopifnot(NROW(data) == rbm@n_inputs)
	
	## Reassign input biases to training example requencies?!  As suggested in: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
	rbm@bias_inputs <- as.real(rowSums(data)/NCOL(data))
	
	## Pass to C for training.
    .Call("train_rbm_R", rbm, as.real(data), as.integer(n_epocs), as.integer(n_threads), package="Rdbn") 
})

## Returns the output of the logicstic function.
#setGeneric("logistic_function", def=function(rbm, vect_val) {standardGeneric("logistic_function")} )
#setMethod("logistic_function", c(rbm="rbm"), 
logistic_function <- function(vect_val) {
    return(1/(1+exp(-vect_val)))
}#)


#     x %*% y
