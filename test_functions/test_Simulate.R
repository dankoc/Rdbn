##
## Simiulate data to test DBN.  This is designed to be a VERY easy problem.  It should get 100% correct.
require(Rdbn)

Classes <- c("A", "B", "C", "D")
y <- rep(Classes, 250) #sample(Classes, 1000, replace=TRUE)

sample_on <- function(n) { rnorm(n, 0.75, sd=1) }
sample_off <- function(n) { rnorm(n, -0.75, sd=1) }

x <- logistic_function(matrix(unlist(lapply(y, function(x){
  if(x==Classes[1]) return(c(sample_on(4), sample_off(12)))
  if(x==Classes[2]) return(c(sample_off(4), sample_on(4), sample_off(8)))
  if(x==Classes[3]) return(c(sample_off(8), sample_on(4), sample_off(4)))  
  if(x==Classes[4]) return(c(sample_off(12), sample_on(4)))
})), nrow= 16))

## RBM.
rbm.obj <- rbm(n_inputs= 16, n_outputs= 4, batch_size=100, cd_n=1, momentum_decay=0.9, weight_cost= 5e-2)
rbm.train(rbm.obj, data= x, n_epocs= 100, n_threads=3)

## Sanity check to make sure that the simulation worked out well.
rowMeans(x[,y=="A"])
rowMeans(x[,y=="B"])
rowMeans(x[,y=="C"])
rowMeans(x[,y=="D"])

## Train a deep belief network.
require(Rdbn)
db <- dbn(layer_sizes= c(16,50,50,100), batch_size=10, cd_n=1, momentum_decay= 0.9, learning_rate=0.1, weight_cost= 5e-2)
db <- dbn.pretrain(db, data= x, n_epocs= 50, n_threads=8)

## Check pre-training using a daydream.
dbn.daydream(db, data=x[,1])

 plot(dbn.daydream(db, data=x[,1], cd_n=1), type="l", ylim=c(0,1))
 points(dbn.daydream(db, data=x[,1], cd_n=2), type="l", col="dark blue")
 points(dbn.daydream(db, data=x[,1], cd_n=5), type="l", col="dark blue")
 points(dbn.daydream(db, data=x[,1], cd_n=10), type="l", col="gray")
 points(dbn.daydream(db, data=x[,1], cd_n=50), type="l", col="gray")
 points(dbn.daydream(db, data=x[,1], cd_n=100), type="l", col="light gray")

## Check whether the example converted to a useful set in pre-training.
a <- dbn.predict(db, data=x, raw_matrix=TRUE)

## Update learning parameters.
db <- dbn.set_momentum_decay(db, 0.8)
db <- dbn.set_learning_rate(db, 0.03)

## refine model with new learning parameters.
db_refine <- dbn.refine(db, data= x, labels= y, n_epocs=100, rate_mult=5, n_threads=1)

val <- dbn.predict(db_refine, data=x)
mat <- dbn.predict(db_refine, data=x)

## Not much better than guessing.
print(paste("Performance: ", sum(val == y)/NROW(y)))

###
## Which neurons are predictive of each class?... 
th <- 0.3
which(rowSums(a[,(y == "A")])/sum(y == "A") > th)
which(rowSums(a[,(y == "B")])/sum(y == "B") > th)
which(rowSums(a[,(y == "C")])/sum(y == "C") > th)
which(rowSums(a[,(y == "D")])/sum(y == "D") > th)
(rowSums(a)/NROW(y)*1000)#[which(rowSums(a[,(y == "A")])/sum(y == "A") > th)]

## Use SVM.
require(e1071)
asvm <- svm(t(x), as.factor(y)) ## SVM does better with RAW data.
pred_svm <- predict(asvm, t(x))

print(paste("% correct (svm): ", sum(pred_svm == as.character(y))/NROW(y)))

