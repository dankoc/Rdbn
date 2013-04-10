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
    neuron_order="integer",   ## Maps neurons in the last layer to class_levels.

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
                momentum_decay= NA) 
{
  rbm_network <- list()
  
  rbm_network[[1]] <- rbm(n_inputs= layer_sizes[1], n_outputs= layer_sizes[1+1], 
      batch_size=batch_size, learning_rate=learning_rate, cd_n=cd_n, momentum_decay= momentum_decay)
  for(i in 2:(n_layers-1)) {
    rbm_network[[i]] <- dbn_layer(n_inputs= layer_sizes[i], n_outputs= layer_sizes[i+1], 
      batch_size=batch_size, learning_rate=learning_rate, cd_n=cd_n, momentum_decay= momentum_decay)
  }
  new("dbn", 
    n_layers=as.integer(n_layers), 
    layer_sizes=as.integer(layer_sizes), 
    network= rbm_network, 
    learning_rate= as.real(learning_rate), 
    batch_size=as.integer(batch_size), 
    class_levels=character(0), 
    neuron_order=integer(0))
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
  def=function(dbn, data, labels, n_epocs= 1000, n_threads=1) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.refine")
})
  
setMethod("dbn.refine", c(dbn="dbn"), 
  function(dbn, data, labels, n_epocs= 1000, n_threads=1) {
    stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)

    ## Deterimeines which neuron to assign each level for the training factor.
    label <- as.factor(label)
    dbn@class_levels= levels(label)
    .Call("predict_dbn_R", dbn, as.real(data), as.integer(n_threads), package="Rdbn")
	
    .Call("refine_dbn_R", dbn, as.real(data), as.real(labels), as.integer(n_epocs), as.integer(n_threads), package="Rdbn") 
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
