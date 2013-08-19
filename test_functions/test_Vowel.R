require(mlbench)
data(Vowel)
require(Rdbn)

## Scale all variables to [0,1].
x <- t(Vowel[,c(2:10)])
y <- Vowel[,11]
for(i in c(1:(NROW(x)))) {
  x[i,] <- logistic_function(scale(x[i,])*5) ## 5 is an arbitrarily chosen value... 
}
# hist(x[1,]) ## Looks about right.

## Train a deep belief network.
db <- dbn(n_layers= 2, layer_sizes= c(9,20), batch_size=11, cd_n=1, momentum_decay= 0.8, learning_rate=0.1, weight_cost=0.01)
#db <- dbn.pretrain(db, data= x, n_epocs= 50, n_threads=8)

## Update learning parameters.
db <- dbn.set_momentum_decay(db, 0.5)
db <- dbn.set_learning_rate(db, 0.05)

## refine model with new learning parameters.
db_refine <- dbn.refine(db, data= x, labels= y, n_epocs=500, rate_mult=5, n_threads=1)

val <- dbn.predict(db_refine, data=x, raw_matrix=FALSE)
mat <- dbn.predict(db_refine, data=x, raw_matrix=TRUE)


print(sum(val == as.character(y))/NROW(y))

cor.test(dbn.predict(db, data=x, raw_matrix=TRUE)[,1], dbn.predict(db, data=x, raw_matrix=TRUE)[,5])
