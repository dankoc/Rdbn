require(mlbench)
data(Vehicle)
require(Rdbn)

## Scale all variables to [0,1].
x <- t(Vehicle[,c(1:18)])
y <- Vehicle[,19]
for(i in c(1:(NCOL(Vehicle)-1))) {
  #x[i,] <- (Vehicle[,i]-min(Vehicle[,i]))/(max(Vehicle[,i])-min(Vehicle[,i]))
  x[i,] <- logistic_function(scale(Vehicle[,i])*2) ## 5 is an arbitrarily chosen value... 
}

trainIndx <- sample(c(1:NCOL(x)), NCOL(x)*0.8, replace=FALSE)
testIndx <- c(1:NCOL(x))[!(c(1:NCOL(x)) %in% trainIndx)]

## Train a deep belief network.
db <- dbn(n_layers= 3, layer_sizes= c(18,75,50), batch_size=100, cd_n=1, momentum_decay= 0.9, learning_rate=0.1, weight_cost= 0.1)
db <- dbn.pretrain(db, data= x[,trainIndx], n_epocs= 10, n_threads=1)

## refine model with new learning parameters.
db_refine <- dbn.refine(db, data= x[,trainIndx], labels= y[trainIndx], n_epocs=100, rate_mult=10, n_threads=8)
pred_dbn <- dbn.predict(db_refine, data=x[,testIndx], n_threads=1)

print(paste("% correct (dbn): ", sum(pred_dbn == as.character(y[testIndx]))/NROW(y[testIndx])))

## Compare to SVM
require(e1071)
asvm <- svm(t(x[,trainIndx]), y[trainIndx]) ## SVM does better with RAW data.
pred_svm <- predict(asvm, t(x[,testIndx]))

print(paste("% correct (svm): ", sum(pred_svm == as.character(y[testIndx]))/NROW(y[testIndx])))



## BUG ALSO AFFECTS dbn.predict.
 a <- dbn.predict(db_refine, data=x[,testIndx], n_threads=1)
 b <- dbn.predict(db_refine, data=x[,testIndx], n_threads=8)
 c <- dbn.predict(db_refine, data=x[,testIndx], n_threads=8)
 d <- dbn.predict(db_refine, data=x[,testIndx], n_threads=8)
 e <- dbn.predict(db_refine, data=x[,testIndx], n_threads=8)
 
 dbn.predict(db_refine, data=x[,testIndx], n_threads=1) == dbn.predict(db_refine, data=x[,testIndx], n_threads=8)
 
 dbn.predict(db_refine, data=x[,testIndx], n_threads=8, raw_matrix=TRUE)
 