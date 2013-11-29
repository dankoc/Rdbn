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
## Quick & Simple calls to train a classifier using deep belief networks.
db <- dbn(layer_sizes= c(18,75,150), batch_size=10, cd_n=1, momentum_decay= 0.9, learning_rate=0.1, weight_cost= 0.01)
db <- dbn.pretrain(db, data= x[,trainIndx], n_epocs= 5, n_threads=8)

## refine model with new learning parameters.
db@batch_size <- as.integer(10)
db_refine <- dbn.refine(db, data= x[,trainIndx], labels= y[trainIndx], n_epocs=100, rate_mult=5, n_threads=8)
pred_dbn <- dbn.predict(db_refine, data=x[,testIndx], n_threads=8)

print(paste("% correct (dbn): ", sum(pred_dbn == as.character(y[testIndx]))/NROW(y[testIndx])))

