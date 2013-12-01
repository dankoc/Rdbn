
## Read data from csv.
train <- read.table("train.csv.gz", header=TRUE, sep=",")

data <- t(train[,c(2:NCOL(train))])/255
label <- train[,1]

## Train a deep belief network.
require(Rdbn)
db <- dbn(layer_sizes= c(784,50,50), batch_size=100, cd_n=1, momentum_decay= 0.9, learning_rate=0.1, weight_cost= 2e-5)
db <- dbn.pretrain(db, data= data, n_epocs= 100, n_threads=8)

###################################################################
##
## Draw some receptive fields.
##
transformData <- function(dataEntry, ncol=28) { (matrix(dataEntry, ncol= ncol)) }
col <- gray.colors(20, 1, 0)

## Reversed about the horizontal axis b/c lattice starts drawing in lower-left.  Fix eventually...
par(mfrow=c(2,5), mar = c(0.2, 0.2, 0.2, 0.2))
for(i in 0:9) {
  image(transformData(data[,which(label==i)[4]]), axes=FALSE, col=col)
}

## View IO_weights in the same way (these are receptive fields).
par(mfrow=c(7,7), mar = c(0.2, 0.2, 0.2, 0.2))  ## This one looks very useful.
for(i in 1:49) {
  image(transformData(db@network[[1]]@io_weights[,i]), axes=FALSE, col=col)
}

par(mfrow=c(7,7), mar = c(0.2, 0.2, 0.2, 0.2))
for(i in 1:49) {
  image(transformData(db@network[[2]]@io_weights[,i], ncol=7), axes=FALSE, col=col)
}

###################################################################
##
## The output of this receptive field function in ssomewhat less useful, as requiring all 0's imples constraints on the input weights
## that is why everything appears so heavily correlated in this visualization.
##
par(mfrow=c(7,7), mar = c(0.2, 0.2, 0.2, 0.2))
for(i in 1:49) {
  image(transformData(dbn.clamplayer(db, neuron=i, layer=2)), axes=FALSE, col=col)
}

par(mfrow=c(7,7), mar = c(0.2, 0.2, 0.2, 0.2))
for(i in 1:49) {
  image(transformData(dbn.clamplayer(db, neuron=i, layer=3)), axes=FALSE, col=col)
}

###################################################################
##
## Daydream ... Should create 49 copeis of whatever digit is stated.
##              As cd_n iterations increases, there's a chance the 
##              Gibbs sampler will fall into a state representing
##              a different number.
par(mfrow=c(7,7), mar = c(0.2, 0.2, 0.2, 0.2))
for(i in 1:49) {
  image(transformData(dbn.daydream(db, data= data[,which(label==5)[i]], cd_n=500)), axes=FALSE, col=col)
}

###################################################################
##
## Visualizations of the degree to which weights are correlated with one another.
##
net <- db@network[[1]]@io_weights
net[net > 3] <- 3
net[net < -3] <- -3
require(lattice)
levelplot(net)

## And correlations between weights...
cc_iow <- cor(db@network[[1]]@io_weights)
cc_iowt <- cor(t(db@network[[1]]@io_weights))

drawCor <- function(cc) {

	yb.sig.pal <- function(n, scale=10) {
	 ints<- c(0:(n-1))/(n-1)   ## Linear scale from 0:1 x N values.
	 ints<- 1/(1+exp(scale*(0.5-ints)))## Transfer to sigmoidal scale.
	 b<- min(ints)
	 m<- 2*b/(n-1)
	 ints<- ints+(m*c(0:(n-1)) -b)## Transform by linear function to fill colors out to maxes.
	 
	 ## Transfer to colorspace.
	 # Yellow: 255, 255, 0
	 # White:  255, 255, 255
	 # Blue:   0, 0, 255
	 YW <- ints[ints < 0.5] *2
	 WB <- (ints[ints >= 0.5]-0.5) *2
	 YW[YW<0] <- 0; WB[WB>1] <- 1
	 c(rgb(1, 1, YW), rgb(1-WB, 1-WB, 1))
	}

	 library(latticeExtra)
	 hc1 <- hclust(dist(cc, method = "canberra"))
	 hc1 <- as.dendrogram(hc1)
	 ord.hc1 <- order.dendrogram(hc1)

	 pl <- levelplot((cc)[ord.hc1, ord.hc1], col.regions= yb.sig.pal(100, scale=3), xlab="", ylab="", 
		 colorkey = list(space="left", labels=list(cex=1.5)), 
		 legend = list(
			right = list(fun = dendrogramGrob,
				 args = list(x = hc1, ord = ord.hc1, side = "right", #lwd=2,
				 size = 7, size.add = 0.5, type = "rectangle")), 
			top = list(fun = dendrogramGrob,
				 args = list(x = hc1, ord = ord.hc1, side = "top", #lwd=2,
				 size = 1, size.add = 0.5, type = "rectangle"))
				 ))
	 print(pl)
}

drawCor(cc_iow)
drawCor(cc_iowt)
