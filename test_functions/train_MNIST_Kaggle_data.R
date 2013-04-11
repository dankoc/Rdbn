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
db <- dbn(n_layers= 5, layer_sizes= c(784,500,500,250,10), batch_size=100, cd_n=1, momentum_decay= 0.9)
db2 <- dbn.train(db, data= data, n_epocs= 1, n_threads=5)
db3 <- dbn.refine(db2, data=data, labels=label, n_epocs=2, n_threads=3)

q("yes")


 rr <- rbm(n_inputs= 784, n_outputs= 200, batch_size=100, cd_n=1, momentum_decay= 0.9) #NA
 rr2 <- rbm.train(rr, data= data[,c(1:1000)], n_epocs= 1, n_threads=2)

 require(Rdbn)
 pred <- dbn.predict(db2, data[,which(label==1)[c(1:100)]])

 
 labels= label
 dbn <- db2
 n_epocs= 1000
 n_approx= 500
 n_threads=3
 
 
 label <- as.factor(label)
 lablist <- lapply(levels(label), function(x) {rowMeans(dbn.predict(db2, data[,which(label==x)[c(1:100)]]))})
 matrix(unlist(lablist), ncol=10)
 
 