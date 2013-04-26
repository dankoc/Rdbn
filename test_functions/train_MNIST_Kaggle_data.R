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

setwd("/home/dankoc/testData/MNIst_db_handwriting/csv")

## Read data from csv.
train <- read.table("train.csv.gz", header=TRUE, sep=",")
data <- t(train[,c(2:NCOL(train))])/255
label <- train[,1]

## Train a deep belief network.
require(Rdbn)
db <- dbn(n_layers= 4, layer_sizes= c(784,500,500,2000), batch_size=100, cd_n=1, momentum_decay= 0.9, learning_rate=0.1, weight_cost= 2e-5)
db <- dbn.pretrain(db, data= data, n_epocs= 100, n_threads=8)

save.image("pretrained.RData")

## Update learning parameters.
db <- dbn.set_momentum_decay(db, 0.8)
db <- dbn.set_learning_rate(db, 0.03)
db <- dbn.set_weight_cost(db, 5e-5)

## refine model with new learning parameters.
db_refine <- dbn.refine(db, data=data, labels=label, n_epocs=100, rate_mult=5, n_threads=8)

save.image("refined.RData")

pred_dbn <- dbn.predict(db_refine, data=data, n_threads=8)
print(paste("% correct (dbn): ", sum(pred_dbn == as.character(label))/NROW(label)))

