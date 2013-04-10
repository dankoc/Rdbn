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

#    momentum_decay="numeric", ## Momentum_decay
#    use_momentum="logical",   ## Use momentum during fitting.

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
	momentum_decay= as.real(momentum_decay),
    class_levels=character(0))
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
  def=function(dbn, data, labels, n_epocs= 1000, n_approx=500, n_threads=1) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.refine")
})
  
setMethod("dbn.refine", c(dbn="dbn"), 
  function(dbn, data, labels, n_epocs= 1000, n_approx= 500, n_threads=1) {
    stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)

	print("Adding discriminitive layer.")
    ## Add an extra layer, as mentioned here: http://www.scholarpedia.org/article/Deep_belief_nets
    ## Hinton says: "Discriminative fine-tuning can be performed by adding a final layer of variables that represent the desired outputs and backpropagating error derivatives"
	## For this task, initialize weights to something reasonable ... i.e. (P(neuron_is_on|example_output_is_on)-0.5)/(#_neurons_in_previous_layer)
	## See notebook 4-10-12 ...
    labels <- as.factor(labels)
    dbn@class_levels<- levels(labels)
    n_outputs= NROW(dbn@class_levels)

    mm<- matrix(unlist(lapply(dbn@class_levels, function(x) {
      .Call("predict_dbn_R", dbn, as.real(data[,which(labels==x)[c(1:n_approx)]]), as.integer(n_threads), package="Rdbn")
    })), ncol=NROW(dbn@class_levels))
	mm<- (mm-0.5)/dbn@layer_sizes[dbn@n_layers]
    dbn@network[[dbn@n_layers]] <- dbn_layer(n_inputs= dbn@layer_sizes[dbn@n_layers], 
	                                         n_outputs= n_outputs, 
	                                         batch_size=dbn@batch_size, 
	                                         learning_rate=dbn@learning_rate,
	                                         io_weights= mm)
	
	## Increment these variables.
    dbn@layer_sizes <- c(dbn@layer_sizes, n_outputs)
	dbn@n_layers <- dbn@n_layers+1
	
    ## Alternative idea: It may be possible to take a final layer pre-trained using the generative approach (i.e. contrastive divergence)
    ## and choose which 'feature' each output neuron represents through dynamic programming (i.e. a little like Viterbi, but subject to 
    ## a few additional constraints ... See notebook 4-10-13)
    
	## Construct expected output vector.
	label_matrix <- matrix(0, ncol=NROW(labels), nrow=n_outputs)
	label_matrix
	## MAY BE EASIER IN C?!
	
    print("Fine tuning weights using backpropragation.")
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
