require(mlbench)
data(Vehicle)
require(Rdbn)
set.seed(34)

## Scale all variables to [0,1].
x <- t(Vehicle[,c(1:18)])
y <- Vehicle[,19]
for(i in c(1:(NCOL(Vehicle)-1))) {
  x[i,] <- logistic_function(scale(Vehicle[,i])*2) ## 2 is an arbitrarily chosen value... 
}

trainIndx <- sample(c(1:NCOL(x)), NCOL(x)*0.8, replace=FALSE)
testIndx <- c(1:NCOL(x))[!(c(1:NCOL(x)) %in% trainIndx)]

###
## Train an RBM. 
rb <- rbm(n_inputs= 18, n_outputs= 1500, batch_size=100, cd_n=1, momentum_decay= 0.9, learning_rate=0.5, weight_cost= 0.1)
rb <- rbm.train(rb, data= x[,trainIndx], n_epocs= 5, n_threads=8)


