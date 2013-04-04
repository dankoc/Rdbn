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
# e.g.:
# rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
# Initial weights set using S8.1 in: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
dbn <- function(n_layers, layer_sizes) {
  rbm_network <- list()
  for(i in 1:(n_layers-1)) {
    rbm_network[[i]] <- rbm(n_inputs= layer_sizes[i], n_outputs= layer_sizes[i+1])
  }
  new("dbn", n_layers=as.integer(n_layers), layer_sizes=as.integer(layer_sizes), network= rbm_network)
}

 # require(Rdbn)
 # rr <- rbm(n_inputs= as.integer(5), n_outputs= as.integer(10))
 # train(rr, matrix(c(1:10), ncol=5))

#` Method to train a boltzman machine (stored in rbm).
#` @param rbm The boltzman machine.
#` @param data A data matrix wherein each row represents an observation. NCOL(data)= n_inputs.
#` @export
setGeneric("train", 
  def=function(dbn, data, ...) {
	stopifnot(class(dbn) == "dbn")
	standardGeneric("train")
})
  
setMethod("train", c(dbn="dbn"), 
  function(dbn, data, batch_size=10, cdn= 1) { ## Regularization/ LASSO type options?!
#  	stopifnot(NROW(data) == dbn@network[[1]]@n_inputs)
#    .Call("train_rbm_R", rbm, as.real(data), as.integer(batch_size), as.integer(cdn), package="Rdbn") 
})

