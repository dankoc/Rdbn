##
## test_rbm.R -- Test the C implementation of restricted boltzman machines.
##

 require(Rdbn)
 
 ## Make the example from: http://blog.echen.me/2011/07/18/introduction-to-restricted-boltzmann-machines/
 ## Visible units correspond to movies: Harry Potter, Avatar, LOTR3, Gladiator, Titanic, Glitter.
 training_data <- data.frame(HarryPotter=c(1,1,1,0,0,0), Avatar=c(1,0,1,0,0,0), LOTR3=c(1,1,1,1,1,1), 
							Gladiator=c(0,0,0,1,1,1),Titanic=c(0,0,0,1,1,1),Glitter=c(0,0,0,0,0,0))
 row.names(training_data) <- c("Alice","Bob","Carol","David","Eric","Fred")
 data_matrix <- matrix(rep(data.matrix(training_data), 100), ncol=6) ## Convert to a matrix and repeat 100 times (for training).
 
 rr <- rbm(n_inputs= as.integer(6), n_outputs= as.integer(2))
 train(rr, data= data_matrix, batch_size=1, cdn=1)
