##
## test_rbm.R -- Test the C implementation of restricted boltzman machines.
##

 require(Rdbn)
 
 ## Make the example from: http://blog.echen.me/2011/07/18/introduction-to-restricted-boltzmann-machines/
 ## Visible units correspond to movies: Harry Potter, Avatar, LOTR3, Gladiator, Titanic, Glitter.
 training_data <- data.frame(HarryPotter=c(1,1,1,0,0,0), Avatar=c(1,0,1,0,0,0), LOTR3=c(1,1,1,1,1,1), 
							Gladiator=c(0,0,0,1,1,1),Titanic=c(0,0,0,1,1,1),Glitter=c(0,0,0,0,0,0))
 row.names(training_data) <- c("Alice","Bob","Carol","David","Eric","Fred")
 training_data <- t(training_data)
 
 ## Convert to a matrix and repeat 100 times (for training).
 data_matrix <- matrix(data.matrix(training_data), nrow=6)[,sample(c(1:6), 1, replace=TRUE)]
 
 rr <- rbm(n_inputs= 6, n_outputs= 2, batch_size=10, cd_n=1)
 rbm.train(rr, data= data_matrix, n_epocs= 1000)

 #clamp_output(rr, c(0,1)) ## Currently dosen't work...
 
 ## NOTES: 
 # (1) My batch size is affecting my weights.  I don't think that should be so!
 # (2) 
 
 ## Train DBN...
 db <- dbn(n_layers= 3, layer_sizes= c(6,2,2), batch_size=10, cd_n=1)
 dbn.train(db, data= data_matrix, n_epocs= 1000)

 # db <- dbn(n_layers= 5, layer_sizes= c(784,500,500,2000,10))
 