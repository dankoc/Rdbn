##
## Simiulate data to test DBN.  This is designed to be a VERY easy problem.  It should get 100% correct.
require(Rdbn)

Classes <- c("A", "B", "C", "D")
y <- sample(Classes, 1000, replace=TRUE)

sample_on <- function(n) { rnorm(n, 4, sd=1) }
sample_off <- function(n) { rnorm(n, -4, sd=1) }

x <- logistic_function(matrix(unlist(lapply(y, function(x){
  if(x==Classes[1]) return(c(sample_on(4), sample_off(12)))
  if(x==Classes[2]) return(c(sample_off(4), sample_on(4), sample_off(8)))
  if(x==Classes[3]) return(c(sample_off(8), sample_on(4), sample_off(4)))  
  if(x==Classes[4]) return(c(sample_off(12), sample_on(4)))
})), nrow= 16))

## Sanity check to make sure that the simulation worked out well.
rowMeans(x[,y=="A"])
rowMeans(x[,y=="B"])
rowMeans(x[,y=="C"])
rowMeans(x[,y=="D"])

## Train a deep belief network.
require(Rdbn)
db <- dbn(n_layers= 4, layer_sizes= c(16,50,50,100), batch_size=100, cd_n=1, momentum_decay= 0.99, learning_rate=0.1)
db <- dbn.train(db, data= x, n_epocs= 100, n_threads=8)

save.image("~/test_Simulate.RData")

load("~/test_Simulate.RData")
require(Rdbn)

## Update learning parameters.
db <- dbn.set_momentum_decay(db, 0.8)
db <- dbn.set_learning_rate(db, 0.03)

## refine model with new learning parameters.
db_refine <- dbn.refine(db, data= x, labels= y, n_epocs=10, rate_mult=5, n_threads=1)

val <- dbn.predict(db_refine, data=x, raw_matrix=FALSE)
mat <- dbn.predict(db_refine, data=x, raw_matrix=TRUE)

## Perfect performance! :)
print(paste("Performance: ", sum(val == y)/NROW(y)))


## Some additional sanity checks here. #################################################
## Works fine.
# which(dbn.predict(db, x[,y=="A"][,1], raw_matrix=TRUE)>0.5) #[1]  8 32 77 85
# which(dbn.predict(db, x[,y=="A"][,2], raw_matrix=TRUE)>0.5) #[1]  8 32 77 85
# which(dbn.predict(db, x[,y=="A"][,3], raw_matrix=TRUE)>0.5) #[1]  8 32 77 85
# which(dbn.predict(db, x[,y=="B"][,3], raw_matrix=TRUE)>0.5) #[1]  6 10 13 19 21 49 71 73 98
# which(dbn.predict(db, x[,y=="C"][,3], raw_matrix=TRUE)>0.5) #[1]  8 49 58 72 90 95 96
# which(dbn.predict(db, x[,y=="D"][,3], raw_matrix=TRUE)>0.5) #[1]  7  8 12 45 49 73 84

# cor.test(dbn.predict(db, data=x, raw_matrix=TRUE)[,y=="A"][,3], dbn.predict(db, data=x, raw_matrix=TRUE)[,y=="A"][,1])$estimate[[1]] #0.9999505
# cor.test(dbn.predict(db, data=x, raw_matrix=TRUE)[,y=="B"][,3], dbn.predict(db, data=x, raw_matrix=TRUE)[,y=="A"][,1])$estimate[[1]] #0.04404162

## All 4 outputs light up, indicating problem with backprop.
# which(dbn.predict(db_refine, x[,y=="A"][,1], raw_matrix=TRUE)>0.5) #[1] 1 2 3 4
