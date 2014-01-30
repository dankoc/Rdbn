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
                      batch_size=10, 
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
    stopifnot(weight_cost>=0)
    use_l2_penalty=TRUE
  }

  new("dbn_layer", 
    n_inputs=as.integer(n_inputs), 
    n_outputs=as.integer(n_outputs), 
    io_weights=io_weights, 
    bias_outputs= bias_outputs,
    learning_rate=as.numeric(learning_rate), 
    cd_n=as.integer(cd_n), 
    batch_size=as.integer(batch_size),
	use_l2_penalty=as.logical(use_l2_penalty),
	weight_cost=as.numeric(weight_cost),
    use_momentum= as.logical(use_momentum), 
    momentum_decay= as.numeric(momentum_decay))
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

################################################################################
#' Creates a new restricted Boltzman machine object.
#'
#' Creates a new restricted Boltzmann machine (RBM) object, with the specified n_inputs
#' and n_outputs.
#' 
#' Initial weights set to Normally distributed mean= 0, var= 0.01 (as suggested in using S8.1 of:
#' http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf)
#'
#' @param n_inputs Description of \code{n_inputs}.  An object of type rbm to be trained.
#' 
#' @param data Description of \code{data}.  The training data.  Training examples 
#'	are passed as columns (but specifying rows will work also, provided that the
#'  matrix is not square).
#'
#' @param n_epocs Description of \code{n_epocs}.  Number of 'epocs', or loops 
#'  through the training set to make.  Default: 50.
#' 
#' @param n_threads Description of \code{n_threads}.  Executes the specified
#'  number of threads on a POSTIX system.  Default: 1.
#'
#' @examples
#' rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
rbm <- function(n_inputs, 
                n_outputs, 
                x= NULL,				
                batch_size=10,
                learning_rate=0.1, 
                cd_n=1, 
                momentum_decay= NA, 
                weight_cost= NA,
                io_weights=NULL, 
                bias_inputs=NULL, 
                bias_outputs=NULL, ...) 
{

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
      learning_rate=as.numeric(learning_rate), 
      cd_n=as.integer(cd_n), 
      batch_size=as.integer(batch_size),
      momentum_decay= as.numeric(momentum_decay)),
    bias_inputs= as.numeric(bias_inputs))
  
  if(!is.null(x)) {
    rbm <- rbm.train(rbm, data= x, ...)
  }
  
  return(rbm)
}

################################################################################
#' Train a restricted Boltzman machine.
#'
#' Trains a restricted Boltzmann machine (RBM) using 'contrastive divergence'.  
#'
#' @param rbm Description of \code{rbm}.  An object of type rbm to be trained.
#' 
#' @param data Description of \code{data}.  The training data.  Training examples 
#'	are passed as columns (but specifying rows will work also, provided that the
#'  matrix is not square).
#'
#' @param n_epocs Description of \code{n_epocs}.  Number of 'epocs', or loops 
#'  through the training set to make.  Default: 50.
#' 
#' @param n_threads Description of \code{n_threads}.  Executes the specified
#'  number of threads on a POSTIX system.  Default: 1.
#'
#' @export
#' @docType methods
#' @rdname rbm.train
#' 
#' @examples
#' require(Rdbn)
#'
#' # Simulate some data to test.
#' Classes <- c("A", "B", "C", "D")
#' y <- sample(Classes, 1000, replace=TRUE)
#'
#' sample_on <- function(n) { rnorm(n, 1.5, sd=1) }
#' sample_off <- function(n) { rnorm(n, -1.5, sd=1) }
#'
#' x <- logistic_function(matrix(unlist(lapply(y, function(x){
#'   if(x==Classes[1]) return(c(sample_on(4), sample_off(12)))
#'   if(x==Classes[2]) return(c(sample_off(4), sample_on(4), sample_off(8)))
#'   if(x==Classes[3]) return(c(sample_off(8), sample_on(4), sample_off(4)))  
#'   if(x==Classes[4]) return(c(sample_off(12), sample_on(4)))
#' })), nrow= 16))
#'
#' # Make a new RBM object and train.
#' rbm.obj <- rbm(n_inputs= 16, n_outputs= 4, batch_size=100, cd_n=1, momentum_decay=0.9, weight_cost= 5e-2)
#' rbm.train(rbm.obj, data= x, n_epocs= 100, n_threads=3)
setGeneric("rbm.train", 
  def=function(rbm, data, n_epocs= 50, n_threads=1) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("rbm.train")
})
 
#' @rdname rbm.train
#' @aliases helloworld,character,ANY-method
setMethod("rbm.train", c(rbm="rbm"), 
  function(rbm, data, n_epocs= 50, n_threads=1) { ## Regularization/ LASSO type options?!
    if(NROW(data) != rbm@n_inputs) data <- t(data) ## Take transpose, if columns don't represent inputs.
  	stopifnot(NROW(data) == rbm@n_inputs)
	
	## Reassign input biases to training example requencies?!  As suggested in: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
	rbm@bias_inputs <- as.numeric(rowSums(data)/NCOL(data))
	
    .Call("train_rbm_R", rbm, as.numeric(data), as.integer(n_epocs), as.integer(n_threads), package="Rdbn") 
})

clampInput <- function(rbm, y, n=1) {
	matrix(unlist(lapply(1:n, function(i) {
	logistic_function((y[,i] %*% rbm@io_weights)+rbm@bias_outputs)
	})), nrow=rbm@n_outputs)
}

#Sanity check in R
clampOutput <- function(rbm, x, n=1) {
	matrix(unlist(lapply(1:n, function(i) {
	logistic_function((x[,i] %*% t(rbm@io_weights)) + rbm@bias_inputs) 
	})), ncol=rbm@n_inputs)
}

## Clamp Input
setGeneric("rbm.clamp_input", 
  def=function(rbm, data) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("rbm.clamp_input")
})
setMethod("rbm.clamp_input", c(rbm="rbm"), 
  function(rbm, data) {
    if(NCOL(data)== rbm@n_inputs & NROW(data)!= rbm@n_inputs)
       data <- t(data)
   	 stopifnot(NROW(data) == rbm@n_inputs)

    clampInput(rbm, data, n= NCOL(data)) ## Alternatively, this could go to C.
})

## Clamp Output
setGeneric("rbm.clamp_output", 
  def=function(rbm, data) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("rbm.clamp_output")
})

setMethod("rbm.clamp_output", c(rbm="rbm"), 
  function(rbm, data) {
    if(NCOL(data)== rbm@n_outputs & NROW(data)!= rbm@n_outputs)
       data <- t(data)
   	 stopifnot(NROW(data) == rbm@n_outputs)

    clampOutput(rbm, data, n= NCOL(data))
})


## Daydream
setGeneric("rbm.daydream", 
  def=function(rbm, data, cd_n) {
	stopifnot(class(rbm) == "rbm")
	standardGeneric("rbm.daydream")
})
setMethod("rbm.daydream", c(rbm="rbm"), 
  function(rbm, data, cd_n) {
    ## ERROR CHECKING.
	for(i in 1:cd_n) {
      data <- clampInput(rbm, data, n)
	  data <- clampOutput(rbm, data, n)
	}
	
	return(data)
})


