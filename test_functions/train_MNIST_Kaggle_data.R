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

## Balance training set -- Use 10 examples of each image during each mini-batch.  
min_examples <- min(summary(as.factor(label)))

data <- t(train[,c(2:NCOL(train))])/255
label <- train[,1]

## Train a deep belief network.
require(Rdbn)
db <- dbn(n_layers= 4, layer_sizes= c(784,500,500,2000), batch_size=100, cd_n=1, momentum_decay= 0.9, learning_rate=0.1, weight_cost= 2e-5)
db <- dbn.pretrain(db, data= data, n_epocs= 100, n_threads=8)

## Update learning parameters.
db <- dbn.set_momentum_decay(db, 0.8)
db <- dbn.set_learning_rate(db, 0.03)
db <- dbn.set_weight_cost(db, 5e-5)

## refine model with new learning parameters.
db_refine <- dbn.refine(db, data=data, labels=label, n_epocs=100, rate_mult=5, n_threads=8)

save.image("refined.RData")

require(Rdbn)
load("refined.RData")
pred_dbn <- dbn.predict(db_refine, data=data[,c(1:10000)], n_threads=8)
print(paste("% correct (dbn): ", sum(pred_dbn == as.character(label[c(1:10000)]))/NROW(label[c(1:10000)])))

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
summary(as.factor(label)[pred!=label])/summary(as.factor(label))
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


