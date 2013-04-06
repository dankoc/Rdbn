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
    n_layers="integer",     ## Number of network layers, including input and output.
    layer_sizes="integer",  ## Integer vector, representing the number of nodes in layers 1 (input/ visible) .. n (output/ hidden).
    network="list"          ## A list comprised of RBMs.  Indexed from the input to output layer.
  ),
)

# constructor.
dbn <- function(n_layers, layer_sizes, batch_size=1, learning_rate=0.1, cd_n=1, momentum_decay= NA) {
  rbm_network <- list()
  for(i in 1:(n_layers-1)) {
    rbm_network[[i]] <- rbm(n_inputs= layer_sizes[i], n_outputs= layer_sizes[i+1], 
      batch_size=batch_size, learning_rate=learning_rate, cd_n=cd_n, momentum_decay= momentum_decay)
  }
  new("dbn", n_layers=as.integer(n_layers), layer_sizes=as.integer(layer_sizes), network= rbm_network)
}

 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to train a boltzman machine (stored in rbm).
#` @param dbn The layered network of RBMs.
#` @param data A data matrix wherein each column represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("dbn.train", 
  def=function(dbn, data, ...) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("dbn.train")
})
  
setMethod("dbn.train", c(dbn="dbn"), 
  function(dbn, data) {
  	stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)
    .Call("train_dbn_R", dbn, as.real(data), package="Rdbn") 
})

