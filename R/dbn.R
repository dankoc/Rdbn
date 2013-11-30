## Boltzman Machine.
##

#` An S4 class that stores a basic boltzman machine.
#` @slot n_inputs The number of input nodes.
#` @slot n_outputs The number of output nodes.
#` @slot learning_rate The learning rate.
#` @slot io_weights A matrix of weights.
#` @export
setClass("dbn",#"restricted_boltzman_machine", 
  representation(
    n_layers="integer",       ## Number of layers of neurons, including input and output.  Number of RBMS == (n_layers-1).
    layer_sizes="integer",    ## Integer vector, representing the number of nodes in layers 1 (input/ visible) .. n (output/ hidden).
    network="list",           ## A list comprised of RBMs.  Indexed from the input to output layer.
	
    class_levels="character", ## Levels of a factor passed as training data.

    learning_rate="numeric",  ## Learning rate for the deep belief network; used during backpropagation.
    batch_size="integer"      ## Size of the mini-batch to use during backpropagation.
  ),
)

# constructor.
dbn <- function(layer_sizes, 
                batch_size=10, 
                learning_rate=0.1, 
                cd_n=1, 
                momentum_decay= NA,
                weight_cost=NA,

                # The following are defined to allow training during the S4 object construction.
                x= NULL,				
                y= NULL,				
                n_epocs=50,
                rate_mult=5,
                n_threads=1) 
{
  rbm_network <- list()
  n_layers <- as.integer(length(layer_sizes))
  
  rbm_network[[1]] <- rbm(n_inputs= layer_sizes[1], n_outputs= layer_sizes[1+1], 
      batch_size=batch_size, learning_rate=learning_rate, cd_n=cd_n, momentum_decay= momentum_decay, weight_cost=weight_cost)

  if(n_layers>2) 
    for(i in 2:(n_layers-1)) {
      rbm_network[[i]] <- dbn_layer(n_inputs= layer_sizes[i], n_outputs= layer_sizes[i+1], 
      batch_size=batch_size, learning_rate=learning_rate, cd_n=cd_n, momentum_decay= momentum_decay, weight_cost=weight_cost)
    }
  
  dbn <- new("dbn", 
    n_layers=as.integer(n_layers), 
    layer_sizes=as.integer(layer_sizes), 
    network= rbm_network, 
    class_levels=character(0),
    learning_rate= as.numeric(learning_rate), 
    batch_size=as.integer(batch_size))

  if(!is.null(x)) {
    dbn <- dbn.pretrain(dbn, data= x, n_epocs= n_epocs, n_threads= n_threads)
    if(!is.null(y)) {
	  dbn <- dbn.refine(dbn, data= x, labels= y, n_epocs= n_epocs, rate_mult= rate_mult, n_threads= n_threads)
    }
  }

  return(dbn)
}

 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to train a boltzman machine (stored in rbm).
#` @param dbn The layered network of RBMs.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("dbn.pretrain", 
  def=function(dbn, data, n_epocs= 1000, n_threads=1) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.pretrain")
})
  
setMethod("dbn.pretrain", c(dbn="dbn"), 
  function(dbn, data, n_epocs= 1000, n_threads=1) {
    if(NCOL(data)== dbn@network[[1]]@n_inputs & NROW(data)!= dbn@network[[1]]@n_inputs) 
      data <- t(data)
  	stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)
    .Call("train_dbn_R", dbn, as.numeric(data), as.integer(n_epocs), as.integer(n_threads), package="Rdbn") 
})


 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to preform alternating Gibbs sampling on the inner-most RBM layer.
#` @param dbn The layered network of RBMs.
#` @param data A data vector representing [a] traning case(s).
#` @param cd_n Number of iterations of contrastive divergence Gibbs sampling to run in the top-level RBM.
#` @param n_threads Number of concurrent threads to run.
#` @export
setGeneric("dbn.daydream", 
  def=function(dbn, data, cd_n= 1000, n_threads=1) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.daydream")
})
  
setMethod("dbn.daydream", c(dbn="dbn"), 
  function(dbn, data, cd_n= 1000, n_threads=1) {
    if(NCOL(data)== dbn@network[[1]]@n_inputs & NROW(data)!= dbn@network[[1]]@n_inputs) 
      data <- t(data)
  	stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)
    .Call("daydream_dbn_R", dbn, as.numeric(data), as.integer(cd_n), as.integer(n_threads), package="Rdbn") 
})


#` Clamps output layer and runs the network downward to get the receptive rields.
#` @param dbn The layered network of RBMs.
#` @param data A data vector representing [a] output case(s).
#` @param n_threads Number of concurrent threads to run.
#` @export
setGeneric("dbn.receptivefields", 
  def=function(dbn, data, n_threads=1) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.receptivefields")
})
  
setMethod("dbn.receptivefields", c(dbn="dbn"), 
  function(dbn, data, n_threads=1) {
    if(NCOL(data)== dbn@network[[dbn@n_layers-1]]@n_outputs & NROW(data)!= dbn@network[[dbn@n_layers-1]]@n_outputs) 
      data <- t(data)
  	stopifnot(NROW(data) == dbn@network[[dbn@n_layers-1]]@n_outputs)
    .Call("receptivefields_dbn_R", dbn, as.numeric(data), as.integer(n_threads), package="Rdbn") 
})

#` Refines a discriminitive model from the DBN by adding a top layer and training weights using backpropagation.
#` @param dbn The layered network of RBMs.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("dbn.refine", 
  def=function(dbn, data, labels, n_epocs= 1000, rate_mult=5, n_threads=1) { #n_approx=500, 
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.refine")
})
  
setMethod("dbn.refine", c(dbn="dbn"), 
  function(dbn, data, labels, n_epocs= 1000, rate_mult=5, n_threads=1) { #n_approx=500,
    if(NCOL(data)== dbn@network[[1]]@n_inputs & NROW(data)!= dbn@network[[1]]@n_inputs)
      data <- t(data)
    stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)

#    print("Adding discriminitive layer.")
    ## Add an extra layer, as mentioned here: http://www.scholarpedia.org/article/Deep_belief_nets
    ## Hinton says: "Discriminative fine-tuning can be performed by adding a final layer of variables that represent the desired outputs and backpropagating error derivatives"
    labels <- as.factor(labels)
    dbn@class_levels<- levels(labels)
    n_outputs= NROW(dbn@class_levels)

    ## Initalize a new top layer. NOTE: n_rmbs = n_layers-1
    dbn@network[[dbn@n_layers]] <- dbn_layer(n_inputs= dbn@layer_sizes[dbn@n_layers], 
	                                         n_outputs= n_outputs, 
	                                         batch_size=dbn@batch_size, 
	                                         learning_rate=dbn@learning_rate*rate_mult)
	
	## Increment these variables.
    dbn@layer_sizes <- as.integer(c(dbn@layer_sizes, n_outputs))
	dbn@n_layers <- as.integer(dbn@n_layers+1)
	
#    print("Fine tuning weights using backpropragation.")
    .Call("backpropagation_dbn_R", dbn, as.numeric(data), as.integer(labels), as.integer(n_epocs), as.integer(n_threads), package="Rdbn") 
})


 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to train a boltzman machine (stored in rbm).
#` @param dbn The layered network of RBMs.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("dbn.predict", 
  def=function(dbn, data, raw_matrix=FALSE, n_threads=1) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.predict")
})
  
setMethod("dbn.predict", c(dbn="dbn"), 
  function(dbn, data, raw_matrix=FALSE, n_threads=1) {
  	stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)
    pred_matrix <- .Call("predict_dbn_R", dbn, as.numeric(data), as.integer(n_threads), package="Rdbn") 
    if(raw_matrix) 
      return(pred_matrix)
    else 
      pred_classes <- .Call("convert_to_max_R", dbn, as.numeric(pred_matrix), package="Rdbn") 
	
    pred_classes <- as.factor(sapply(pred_classes, function(x) {dbn@class_levels[x]}))
    return(pred_classes)
})



#` Method to train a boltzman machine (stored in rbm).
#` @param dbn The layered network of RBMs.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
#` NTOE: learning_rate can either be a vector w/ n_elemtns= n_rbms, or a single value.
setGeneric("dbn.set_learning_rate", 
  def=function(dbn, learning_rate) {
	standardGeneric("dbn.set_learning_rate")
})
  
setMethod("dbn.set_learning_rate", c(dbn="dbn"), 
  function(dbn, learning_rate) {
  	stopifnot(NROW(learning_rate) == 1 | NROW(learning_rate) == (dbn@n_layers-1)) ## Check assumptions on number of elements.

    dbn@learning_rate<- learning_rate
    if(NROW(learning_rate)== 1) learning_rate <- rep(learning_rate, dbn@n_layers-1) ## Force to a vector.
    for(i in c(1:(dbn@n_layers-1))) { ## Set learning rate at each DBN!
      dbn@network[[i]]@learning_rate= as.numeric(learning_rate[i])
    }
    return(dbn)
})



#` Method to train a boltzman machine (stored in rbm).
#` @param dbn The layered network of RBMs.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
#` NTOE: learning_rate can either be a vector w/ n_elemtns= n_rbms, or a single value.
setGeneric("dbn.set_momentum_decay", 
  def=function(dbn, momentum_decay) {
	standardGeneric("dbn.set_momentum_decay")
})
  
setMethod("dbn.set_momentum_decay", c(dbn="dbn"), 
  function(dbn, momentum_decay) {
  	stopifnot(NROW(momentum_decay) == 1 | NROW(momentum_decay) == (dbn@n_layers-1)) ## Check assumptions on number of elements.

    if(NROW(momentum_decay)== 1) momentum_decay <- rep(momentum_decay, dbn@n_layers-1) ## Force to a vector.
    for(i in c(1:(dbn@n_layers-1))) { ## Set learning rate at each DBN!
      dbn@network[[i]]@use_momentum= !is.na(momentum_decay[i])#TRUE if specified.
      dbn@network[[i]]@momentum_decay= as.numeric(momentum_decay[i])
    }
    return(dbn)
})


#` Method to train a boltzman machine (stored in rbm).
#` @param dbn The layered network of RBMs.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
#` NTOE: learning_rate can either be a vector w/ n_elemtns= n_rbms, or a single value.
setGeneric("dbn.set_weight_cost", 
  def=function(dbn, weight_cost) {
	standardGeneric("dbn.set_weight_cost")
})
  
setMethod("dbn.set_weight_cost", c(dbn="dbn"), 
  function(dbn, weight_cost) {
  	stopifnot(NROW(weight_cost) == 1 | NROW(weight_cost) == (dbn@n_layers-1)) ## Check assumptions on number of elements.

    if(NROW(weight_cost)== 1) weight_cost <- rep(weight_cost, dbn@n_layers-1) ## Force to a vector.
    for(i in c(1:(dbn@n_layers-1))) { ## Set learning rate at each DBN!
      dbn@network[[i]]@use_l2_penalty= !is.na(weight_cost[i])
      dbn@network[[i]]@weight_cost= as.numeric(weight_cost[i])
    }
    return(dbn)
})

