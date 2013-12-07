require(mlbench)
data(Vehicle)
require(Rdbn)

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
set.seed(34) ## Different starting points can result in different performance.
db <- dbn(x= x[,trainIndx], y= y[trainIndx], layer_sizes= c(18,100,150), batch_size=10, momentum_decay= 0.9, learning_rate=0.1, weight_cost= 1e-4, n_threads=8)
pred_dbn <- dbn.predict(db, data=x[,testIndx], n_threads=1)

print(paste("% correct (dbn): ", sum(pred_dbn == as.character(y[testIndx]))/NROW(y[testIndx])))

###
## Alternatively, network training strategies can be applied independently.  
## This provides additional control over training parameters, and can result in better performance.
set.seed(34)
db <- dbn(layer_sizes= c(18,100,150), batch_size=10, cd_n=1, momentum_decay= 0.9, learning_rate=0.1, weight_cost= 1e-4)
db <- dbn.pretrain(db, data= x[,trainIndx], n_epocs=50, n_threads=8)

## refine model with new learning parameters.
db <- dbn.set_learning_rate(db, 0.03)
db_refine <- dbn.refine(db, data= x[,trainIndx], labels= y[trainIndx], n_epocs=300, rate_mult=5, n_threads=1)
pred_dbn <- dbn.predict(db_refine, data=x[,testIndx], n_threads=8)

print(paste("% correct (dbn): ", sum(pred_dbn == as.character(y[testIndx]))/NROW(y[testIndx])))

## For reference, compare to SVM
require(e1071)
asvm <- svm(t(x[,trainIndx]), y[trainIndx]) ## SVM does better with RAW data.
pred_svm <- predict(asvm, t(x[,testIndx]))

print(paste("% correct (svm): ", sum(pred_svm == as.character(y[testIndx]))/NROW(y[testIndx])))

