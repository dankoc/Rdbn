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
 data_matrix <- matrix(data.matrix(training_data), nrow=6)[,sample(c(1:6), 100, replace=TRUE)]
 
 rr <- rbm(n_inputs= 6, n_outputs= 2, batch_size=100, cd_n=1, momentum_decay=0.9, weight_cost= 5e-5) #NA
 rr <- rbm.train(rr, data= data_matrix, n_epocs= 10000, n_threads=1)

 clampInput(rr, training_data, n=6)
 clampOutput(rr, out, n=6)
 sum(clampOutput(rr, out, n=6)-training_data)
