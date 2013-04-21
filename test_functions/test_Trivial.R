## Trivial test -- Meant to be a source of bugchecking in a 
##                 simple example.
require(Rdbn)
set.seed(20)
x <- matrix(c(rep(c(1,0),5), rep(c(0,1),5)), nrow=2)
y <- c(rep("A",5),rep("B",5))

db <- dbn(n_layers= 2, layer_sizes= c(2,2,2), batch_size=10)
db <- dbn.pretrain(db, data= x)

set.seed(20)
db1 <- dbn.refine(db, data= x, labels= y, n_threads=1, n_epocs= 1)

set.seed(20)
db5 <- dbn.refine(db, data= x, labels= y, n_threads=5, n_epocs= 1)

db5

