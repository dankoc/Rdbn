require(mlbench)
data(Vehicle)

## Scale all variables to [0,1].
x <- t(Vehicle[,c(1:18)])
y <- Vehicle[,19]
for(i in c(1:(NCOL(Vehicle)-1))) {
  x[i,] <- (Vehicle[,i]-min(Vehicle[,i]))/(max(Vehicle[,i])-min(Vehicle[,i]))
}

## Train a deep belief network.
require(Rdbn)
db <- dbn(n_layers= 4, layer_sizes= c(18,50,50,100), batch_size=100, cd_n=1, momentum_decay= 0.99, learning_rate=0.1)
db <- dbn.train(db, data= x, n_epocs= 100, n_threads=8)

save.image("~/test_Vehicle.RData")

load("~/test_Vehicle.RData")
require(Rdbn)

## Update learning parameters.
db <- dbn.set_momentum_decay(db, 0.8)
db <- dbn.set_learning_rate(db, 0.03)

## refine model with new learning parameters.
db_refine <- dbn.refine(db, data= x, labels= y, n_epocs=100, rate_mult=5, n_threads=1)

