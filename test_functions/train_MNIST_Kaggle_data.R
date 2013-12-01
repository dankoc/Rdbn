##
## Kaggle data description: 
##
## Classify handwritten digits in this "Getting Started" competition.
## 
## This competition is the first in a series of tutorial competitions designed to introduce 
## people to Machine Learning.
##
## The goal in this competition is to take an image of a handwritten single digit, and determine 
## what that digit is.  As the competition progresses, we will release tutorials which explain 
## different machine learning algorithms and help you to get started.
##
## The data for this competition were taken from the MNIST dataset. The MNIST ("Modified National 
## Institute of Standards and Technology") dataset is a classic within the Machine Learning 
## community that has been extensively studied.  More detail about the dataset, including Machine 
## Learning algorithms that have been tried on it and their levels of success, can be found at 
## http://yann.lecun.com/exdb/mnist/index.html.

## Read data from csv.
train <- read.table("train.csv.gz", header=TRUE, sep=",")

data <- t(train[,c(2:NCOL(train))])/255
## NOW LOGISTIC TRANSFORM THE DATA.  WTF ARE YOU DOING?
label <- train[,1]

## Balance training set -- Use 10 examples of each image during each mini-batch.  
#min_examples <- min(summary(as.factor(label)))

## Train a deep belief network.
require(Rdbn)
db <- dbn(layer_sizes= c(784,500,500,2000), batch_size=100, cd_n=1, momentum_decay= 0.9, learning_rate=0.1)#, weight_cost= 2e-5)
db <- dbn.pretrain(db, data= data, n_epocs= 100, n_threads=8)

save.image("pretrain.RData")

## Update learning parameters.
db <- dbn.set_momentum_decay(db, 0.8)
db <- dbn.set_learning_rate(db, 0.03)
#db <- dbn.set_weight_cost(db, 5e-5)

## refine model with new learning parameters.
db_refine <- dbn.refine(db, data=data, labels=label, n_epocs=100, rate_mult=5, n_threads=8)

save.image("refined.RData")

require(Rdbn)
load("refined.RData")
pred_dbn <- dbn.predict(db_refine, data=data[,c(1:10000)], n_threads=8)
print(paste("% correct (dbn): ", sum(pred_dbn == as.character(label[c(1:10000)]))/NROW(label[c(1:10000)])))

q("no")

## Draw some receptive fields.

require(lattice)
transformData <- function(dataEntry) { (matrix(dataEntry, ncol= 28)) }
levelplot(transformData(data[,which(label==9)[5]]))  ## Reversed about the horizontal axis b/c lattice starts drawing in lower-left.  Fix eventually...
levelplot(transformData(dbn.daydream(db, data[,which(label==5)[1]], cd_n = 1)))

levelplot(transformData(dbn.receptivefields(db_refine, neuron=5, layer=2)))


## Simple visualization of weights.
net <- db@network[[2]]@io_weights
net[net > 3] <- 3
net[net < -3] <- -3
levelplot(net)

cor.test(db@network[[1]]@io_weights[1,],db@network[[1]]@io_weights[2,])
cor.test(db@network[[1]]@io_weights[,1],db@network[[1]]@io_weights[,2])

cor.test(db@network[[2]]@io_weights[1,],db@network[[2]]@io_weights[2,])
cor.test(db@network[[2]]@io_weights[,1],db@network[[2]]@io_weights[,2])

cor.test(db@network[[3]]@io_weights[1,],db@network[[3]]@io_weights[2,])
cor.test(db@network[[3]]@io_weights[,1],db@network[[3]]@io_weights[,2])

###############################################################################
## Still a long ways from the Hinton Science at ~85% accuracy.
##
## Yet when you look at the error types, they make sense.  It's possible that the code is correct, and the model just needs 
## more training, more of the MNIST data, or a more balanced training set to get to the 98.? accuracy level.
##
## TODO: Try balancing the training set.  Once it get's to the mid-90s, I'll be pretty much convinced that everything is working. 
##
## Here's some quick analysis.

## Error rates highest at 5 and 4.
summary(as.factor(label)[pred_dbn!=label])/summary(as.factor(label))
         # 0          1          2          3          4          5          6 
# 0.05372701 0.05785653 0.15465645 0.17191450 0.20898821 0.28722003 0.07541697 
         # 7          8          9 
# 0.14451261 0.15481172 0.15448902 

## Probably not a concidence that these have the lowest amount of training data.
summary(as.factor(label))
   # 0    1    2    3    4    5    6    7    8    9 
# 4132 4684 4177 4351 4072 3795 4137 4401 4063 4188 

## Indeed.
val <- (summary(as.factor(label))-mean(summary(as.factor(label))))/sqrt(var(summary(as.factor(label))))
acc <- summary(as.factor(label)[pred!=label])/summary(as.factor(label))
cor.test(val, acc)

	# Pearson's product-moment correlation

# data:  val and acc 
# t = -2.2518, df = 8, p-value = 0.05442
# alternative hypothesis: true correlation is not equal to 0 
# 95 percent confidence interval:
 # -0.89966105  0.01115467 
# sample estimates:
      # cor 
# -0.622846 

## When it fails to identify a 5, it typically calls it an 8 or a 3.
summary(as.factor(pred)[pred!=label & label==5])/sum(pred!=label & label==5)
         # 0          1          2          3          4          5          6 
# 0.15963303 0.03486239 0.04311927 0.20733945 0.08073394 0.00000000 0.07981651 
         # 7          8          9 
# 0.03211009 0.32018349 0.04220183 

## 4s are overwhelmingly misclassified as 9s.
summary(as.factor(pred)[pred!=label & label==4])/sum(pred!=label & label==4)
          # 0           1           2           3           4           5 
# 0.011750881 0.028202115 0.055229142 0.009400705 0.000000000 0.050528790 
          # 6           7           8           9 
# 0.109283196 0.005875441 0.085781434 0.643948296 

##############################################################################
## From the perspective of the false-call.
## This model overcalls 8 and 9 very frequently.
summary(pred[pred!=label])/summary(as.factor(label)) 
         # 0          1          2          3          4          5          6 
# 0.10963214 0.07792485 0.12808236 0.12364974 0.13040275 0.15362319 0.10031424 
         # 7          8          9 
# 0.08407180 0.28968742 0.25907354 

## When it overcalls an 8, it's often a 9 or a 1.
summary(pred[pred!=label&label==8])/sum(pred!=label&label==8)
         # 0          1          2          3          4          5          6 
# 0.03020668 0.14626391 0.10651828 0.11764706 0.06518283 0.29888712 0.03974563 
         # 7          8          9 
# 0.03656598 0.00000000 0.15898251 

## When it false-calls a 9, it's often a 4, 8, or 7.
summary(pred[pred!=label&label==9])/sum(pred!=label&label==9)
         # 0          1          2          3          4          5          6 
# 0.06182380 0.02163833 0.04482226 0.08037094 0.35394127 0.02627512 0.00618238 
         # 7          8          9 
# 0.19010819 0.21483771 0.00000000 



###############################################################################
## Test the un-refined model for 'classification accuracy'.
##
## Summary: Pre-training dosen't appear to be converging to a useful set of weights.
##          Bug in RBM (?!)
##
n <- 1000
a <- dbn.predict(db, data=data[,c(1:n)], raw_matrix=TRUE)

#####
## Visualize neuron activation patterns | input.
par(mfrow=c(1,1), mar=c(5,4,4,2)+0.1)
plot(rowSums(a[,(label[1:n] == 1)])/sum(label[1:n] == 1), type="l", ylim=c(0,1))
points(rowSums(a[,(label[1:n] == 2)])/sum(label[1:n] == 2), type="l", col="red")
points(rowSums(a[,(label[1:n] == 3)])/sum(label[1:n] == 3), type="l", col="blue")
points(rowSums(a[,(label[1:n] == 4)])/sum(label[1:n] == 4), type="l", col="green")
points(rowSums(a[,(label[1:n] == 5)])/sum(label[1:n] == 5), type="l", col="gray")
points(rowSums(a[,(label[1:n] == 6)])/sum(label[1:n] == 6), type="l", col="orange")
points(rowSums(a[,(label[1:n] == 7)])/sum(label[1:n] == 7), type="l", col="dark red")
points(rowSums(a[,(label[1:n] == 8)])/sum(label[1:n] == 8), type="l", col="dark green")
points(rowSums(a[,(label[1:n] == 9)])/sum(label[1:n] == 9), type="l", col="dark blue")
points(rowSums(a[,(label[1:n] == 0)])/sum(label[1:n] == 0), type="l", col="pink")

par(mfrow=c(10,1), mar=c(1,4,1,2)+0.1)
plot(rowSums(a[,(label[1:n] == 1)])/sum(label[1:n] == 1), type="l", ylim=c(0,1))
plot(rowSums(a[,(label[1:n] == 2)])/sum(label[1:n] == 2), type="l", col="red", ylim=c(0,1))
plot(rowSums(a[,(label[1:n] == 3)])/sum(label[1:n] == 3), type="l", col="blue", ylim=c(0,1))
plot(rowSums(a[,(label[1:n] == 4)])/sum(label[1:n] == 4), type="l", col="green", ylim=c(0,1))
plot(rowSums(a[,(label[1:n] == 5)])/sum(label[1:n] == 5), type="l", col="gray", ylim=c(0,1))
plot(rowSums(a[,(label[1:n] == 6)])/sum(label[1:n] == 6), type="l", col="orange", ylim=c(0,1))
plot(rowSums(a[,(label[1:n] == 7)])/sum(label[1:n] == 7), type="l", col="dark red", ylim=c(0,1))
plot(rowSums(a[,(label[1:n] == 8)])/sum(label[1:n] == 8), type="l", col="dark green", ylim=c(0,1))
plot(rowSums(a[,(label[1:n] == 9)])/sum(label[1:n] == 9), type="l", col="dark blue", ylim=c(0,1))
plot(rowSums(a[,(label[1:n] == 0)])/sum(label[1:n] == 0), type="l", col="pink", ylim=c(0,1))

#####
## Which neurons indicate each number...
th <- 0.95
which(rowSums(a[,(label[1:n] == 1)])/sum(label[1:n] == 1) > th)
which(rowSums(a[,(label[1:n] == 2)])/sum(label[1:n] == 2) > th)
which(rowSums(a[,(label[1:n] == 3)])/sum(label[1:n] == 3) > th)
which(rowSums(a[,(label[1:n] == 4)])/sum(label[1:n] == 4) > th)
which(rowSums(a[,(label[1:n] == 5)])/sum(label[1:n] == 5) > th)
which(rowSums(a[,(label[1:n] == 6)])/sum(label[1:n] == 6) > th)
which(rowSums(a[,(label[1:n] == 7)])/sum(label[1:n] == 7) > th)
which(rowSums(a[,(label[1:n] == 8)])/sum(label[1:n] == 8) > th)
which(rowSums(a[,(label[1:n] == 9)])/sum(label[1:n] == 9) > th)
which(rowSums(a[,(label[1:n] == 0)])/sum(label[1:n] == 0) > th)

#####
## Correlations ... yuck.
cor.test(rowSums(a[,(label[1:n] == 1)]), rowSums(a[,(label[1:n] == 0)]))
cor.test(rowSums(a[,(label[1:n] == 9)]), rowSums(a[,(label[1:n] == 8)]))
cor.test(rowSums(a[,(label[1:n] == 2)]), rowSums(a[,(label[1:n] == 8)]))
cor.test(rowSums(a[,(label[1:n] == 1)]), rowSums(a[,(label[1:n] == 8)]))
cor.test(rowSums(a[,(label[1:n] == 6)]), rowSums(a[,(label[1:n] == 8)]))

#####
## Try taining a classifier.  Naive Bayes should really work very well, if pre-training converged 
## to a useful set of weights.  Yet, no dice.  
require(e1071)
nb_a <- naiveBayes(t(a), label[1:n])

pred_l <- predict(nb_a, t(a)) ## This does not work ... not sure why?!
sum(pred_l == label[1:n])/n

## Poor performance on the first 10.
predict(nb_a, t(a)[c(1:10),], type="raw")
label[1:10]

