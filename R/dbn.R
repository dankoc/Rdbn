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
    n_layers="integer",       ## Number of network layers, including input and output.
    layer_sizes="integer",    ## Integer vector, representing the number of nodes in layers 1 (input/ visible) .. n (output/ hidden).
    network="list",           ## A list comprised of RBMs.  Indexed from the input to output layer.
	
    class_levels="character", ## Levels of a factor passed as training data.
#    neuron_order="integer",   ## Maps neurons in the last layer to class_levels. ## Assume it's in the proper order.

    momentum_decay="numeric", ## Momentum_decay
    use_momentum="logical",   ## Use momentum during fitting.

    learning_rate="numeric",  ## Learning rate for the deep belief network; used during backpropagation.
    batch_size="integer"      ## Size of the mini-batch to use during backpropagation.
  ),
)

# constructor.
dbn <- function(n_layers, 
                layer_sizes, 
                batch_size=1, 
                learning_rate=0.1, 
                cd_n=1, 
                momentum_decay= NA,
                pretraining_momentum_decay= momentum_decay) 
{
  rbm_network <- list()
  
  rbm_network[[1]] <- rbm(n_inputs= layer_sizes[1], n_outputs= layer_sizes[1+1], 
      batch_size=batch_size, learning_rate=learning_rate, cd_n=cd_n, momentum_decay= momentum_decay)
  for(i in 2:(n_layers-1)) {
    rbm_network[[i]] <- dbn_layer(n_inputs= layer_sizes[i], n_outputs= layer_sizes[i+1], 
      batch_size=batch_size, learning_rate=learning_rate, cd_n=cd_n, momentum_decay= momentum_decay)
  }
  
  if(is.na(momentum_decay)) {
    use_momentum=FALSE
  }
  else {
    stopifnot(momentum_decay <= 1 & momentum_decay >= 0) ## Momentum decay between 0 and 1.
    use_momentum=TRUE
  }

  new("dbn", 
    n_layers=as.integer(n_layers), 
    layer_sizes=as.integer(layer_sizes), 
    network= rbm_network, 
    class_levels=character(0),
	momentum_decay= as.real(momentum_decay),
    use_momentum= as.logical(use_momentum),
    learning_rate= as.real(learning_rate), 
    batch_size=as.integer(batch_size))
}

 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to train a boltzman machine (stored in rbm).
#` @param dbn The layered network of RBMs.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("dbn.train", 
  def=function(dbn, data, n_epocs= 1000, n_threads=1) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.train")
})
  
setMethod("dbn.train", c(dbn="dbn"), 
  function(dbn, data, n_epocs= 1000, n_threads=1) {
  	stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)
    .Call("train_dbn_R", dbn, as.real(data), as.integer(n_epocs), as.integer(n_threads), package="Rdbn") 
})


#` Method to train a boltzman machine (stored in rbm).
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
    stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)

#    print("Adding discriminitive layer.")
    ## Add an extra layer, as mentioned here: http://www.scholarpedia.org/article/Deep_belief_nets
    ## Hinton says: "Discriminative fine-tuning can be performed by adding a final layer of variables that represent the desired outputs and backpropagating error derivatives"
    labels <- as.factor(labels)
    dbn@class_levels<- levels(labels)
    n_outputs= NROW(dbn@class_levels)

	## One option: initialize weights to something reasonable ... i.e. (P(neuron_is_on|example_output_is_on)-0.5)/(#_neurons_in_previous_layer)
	## See notebook 4-10-12 ...
#    lablist<- lapply(levels(labels), function(x) {rowMeans(dbn.predict(dbn, data[,which(labels==x)[c(1:n_approx)]], n_threads=n_threads))})
#    mm<- matrix(unlist(lablist), nrow=n_outputs)
#    mm<- (mm-0.5)/dbn@layer_sizes[dbn@n_layers]

    ## Alternative idea: It may be possible to take a final layer pre-trained using the generative approach (i.e. contrastive divergence)
    ## and choose which 'feature' each output neuron represents through dynamic programming (i.e. a little like Viterbi, but subject to 
    ## a few additional constraints ... See notebook 4-10-13)
	
    ## Right now, just using random.
    dbn@network[[dbn@n_layers]] <- dbn_layer(n_inputs= dbn@layer_sizes[dbn@n_layers], 
	                                         n_outputs= n_outputs, 
	                                         batch_size=dbn@batch_size, 
#	                                         io_weights= mm,
	                                         learning_rate=dbn@learning_rate*rate_mult)
	
	## Increment these variables.
    dbn@layer_sizes <- as.integer(c(dbn@layer_sizes, n_outputs))
	dbn@n_layers <- as.integer(dbn@n_layers+1)
	
#    print("Fine tuning weights using backpropragation.")
    .Call("backpropagation_dbn_R", dbn, as.real(data), as.integer(labels), as.integer(n_epocs), as.integer(n_threads), package="Rdbn") 
})


 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to train a boltzman machine (stored in rbm).
#` @param dbn The layered network of RBMs.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("dbn.predict", 
  def=function(dbn, data, n_threads=1) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.predict")
})
  
setMethod("dbn.predict", c(dbn="dbn"), 
  function(dbn, data, n_threads=1) {
  	stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)
    .Call("predict_dbn_R", dbn, as.real(data), as.integer(n_threads), package="Rdbn") 
	
	## TODO: Return a vector here, rather than a matrix.
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
      dbn@network[[i]]@learning_rate= learning_rate[i]
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

    dbn@momentum_decay<- momentum_decay
    if(NROW(momentum_decay)== 1) momentum_decay <- rep(momentum_decay, dbn@n_layers-1) ## Force to a vector.
    for(i in c(1:(dbn@n_layers-1))) { ## Set learning rate at each DBN!
      dbn@network[[i]]@use_momentum= TRUE
      dbn@network[[i]]@momentum_decay= momentum_decay[i]
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
      dbn@network[[i]]@use_l2_penalty= TRUE
      dbn@network[[i]]@weight_cost= weight_cost[i]
    }
    return(dbn)
})

