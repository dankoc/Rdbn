##
## train_nnet.R -- trains a traditional neural network using the 
## backpropagation algorithm.
##

setwd("/usr/projects/GROseq.parser/tss_detecter/")

require(bigWig)
require(nnet)
require(Rdbn)
source("roc.calc.R")

###################################
## Vars. 
n.examp <- 10000
step.size   <- 20

###################################
## Compile data for positive/ neg.

# Position of positive TSS. -- use this for evaluating.
roi <- data.frame(chrom= c("chr16"), start= c(66461199), end= c(68482591))

## Train on whole genome, using Andre's GRO-cap TSS 'predictions' as the true set.
GROcap.tss.bed <- read.table("/usr/projects/GROseq.parser/tss_new/hg19.k562.new_hmm2.bed")
rand.train.tss.bed <- GROcap.tss.bed[sort(sample(which(GROcap.tss.bed[,6]=="+"), n.examp, replace=FALSE)),c(1:3)]  ## Train from just he plus strand ... we have enough, and the negative strand expected to look the same.

# Position of negative TSS set.
rand.train.notss.bed<- data.frame(V1= rep(roi$chrom[1], n.examp), V2= sample(c(roi$start[1]:roi$end[1]), n.examp)) # Draw position of negative TSS at random... (for now).
rand.train.notss.bed$V3 <- rand.train.notss.bed$V2+1

# Combine.
rand.train.bed <- rbind(rand.train.tss.bed, rand.train.notss.bed)
rand.train.classes <- c(rep(1, NROW(rand.train.tss.bed)), rep(0, NROW(rand.train.notss.bed)))

# Get data.
#bw.pos.data <- lapply(bw.files, function(x) {collect.many(bed= rand.tss.bed[,c(1:3)], bigWig.plus= x, halfWindow= window.size, step= step.size)})
#bw.neg.data <- lapply(bw.files, function(x) {collect.many(bed= rand.bed[,c(1:3)], bigWig.plus= x, halfWindow= window.size, step= step.size)})
#bw.data <- rbind(bw.pos.data, bw.neg.data)

# Now make a large test set for evaluating preformance. 
# IDEA: Try to replace this with manually curated set.  Optimize w.r.t. the manually curated set.
rand.test.tss.bed <- GROcap.tss.bed[sort(sample(which(GROcap.tss.bed[,6]=="+"), n.examp, replace=FALSE)),c(1:3)]  ## Evaluate.
rand.test.notss.bed<- data.frame(V1= rep(roi$chrom[1], n.examp), V2= sample(c(roi$start[1]:roi$end[1]), n.examp)) # Draw position of negative TSS at random... (for now).
rand.test.notss.bed$V3 <- rand.test.notss.bed$V2+1
rand.test.bed <- rbind(rand.test.tss.bed, rand.test.notss.bed)
rand.test.classes <- c(rep(1, NROW(rand.test.tss.bed)), rep(0, NROW(rand.test.notss.bed)))

###################################
## Functions to train and evaluate.

## Optimizes number of hidden nodes, and returns the optimal model.
opt.nnet <- function(datapath, x.train.bed, x.predict.bed, y.train, y.predict= y.train, halfWindow= window.size, step= step.size, nhidden= c(1, 5, 10, 20, seq(40, 200, 40)), ...) {

# Get the data.
 bw.data <- load.bigWig(datapath)
 x.train <- collect.many(bed= x.train.bed, bigWig.plus= bw.data, halfWindow= halfWindow, step= step)
 x.predict <- collect.many(bed= x.predict.bed, bigWig.plus= bw.data, halfWindow= halfWindow, step= step)

# Fit a feed-forard neural networks w/ each hidden node size.
 mod <- NULL
 acc <- -1
 for(i in nhidden) {
  n <- nnet(y= y.train, x= x.train, size=i, MaxNWts= 25000, linout= TRUE) ## size -> num. nodes in the hidden layer.
  pred <- predict(n, x.predict)
  pred[pred >  0.5] <- 1
  pred[pred <= 0.5] <- 0
  pred.acc <- (sum(pred == y.predict)/NROW(y.predict))
  if(acc < pred.acc) mod <- n
  print(paste("#:", i, "A:", pred.acc))
 }
 
 ## Plot a ROC plot.
 roc.values <- logreg.roc.calc(y.predict, predict(mod, x.predict))
 roc.plot(roc.values, ...)
 plot(colSums(x.train[y.train == 1,]), ylab="Training data", type="l", ...)
 plot(colSums(x.predict[y.predict == 1,]), ylab="Prediction data", type="l", ...)

# Return the best performing neural network.
 return(mod)
}

###################################
## Data source information.   Trains a model for each data type.
pdf("ROC.meta.plots.pdf")
## TSS. 
#  GRO-cap.
groCap.plus.path  <- "/usr/data/GROseq.parser/hg19/k562/groseq_tss/groseq_tss_wTAP_plus.bigWig" #bw.files[["groCap.plus"]] <-  load.bigWig(groCap.plus.path)#groCap.plus <-
groCap.plus.model <- opt.nnet(datapath= groCap.plus.path, rand.train.bed, rand.test.bed, rand.train.classes, main="GRO-cap Plus")

groCap.minus.path <- "/usr/data/GROseq.parser/hg19/k562/groseq_tss/groseq_tss_wTAP_minus.bigWig" #bw.files[["groCap.minus"]]<- load.bigWig(groCap.minus.path)#groCap.minus <-
groCap.minus.model <- opt.nnet(datapath= groCap.minus.path, rand.train.bed, rand.test.bed, rand.train.classes, main="GRO-cap Minus")

#  Cage.
#cage.plus.path  <- "/usr/data/GROseq.parser/hg19/k562/cage/wgEncodeRikenCageK562NucleusPamPlusSignal.bigWig"
#cage.minus.path <- "/usr/data/GROseq.parser/hg19/k562/cage/wgEncodeRikenCageK562NucleusPamMinusSignal.bigWig"

## GRO-seq.
groSeq.plus.path  <- "/usr/data/GROseq.parser/hg19/k562/groseq/groseq_plus.bigWig" #bw.files[["groSeq.plus"]] <- load.bigWig(groSeq.plus.path)#groSeq.plus <- 
groSeq.plus.model <- opt.nnet(datapath= groSeq.plus.path, rand.train.bed, rand.test.bed, rand.train.classes, main="GRO-seq Plus")

groSeq.minus.path <- "/usr/data/GROseq.parser/hg19/k562/groseq/groseq_minus.bigWig" #bw.files[["groSeq.minus"]]<- load.bigWig(groSeq.minus.path)#groSeq.minus <- 
groSeq.minus.model <- opt.nnet(datapath= groSeq.minus.path, rand.train.bed, rand.test.bed, rand.train.classes, main="GRO-seq Minus")

## Histone mods.
histone.dir <- "/usr/data/GROseq.parser/hg19/k562/histones/"
histone.files <- paste(histone.dir, c("wgEncodeBroadHistoneK562H3k4me3StdSig.bigWig", "wgEncodeBroadHistoneK562H3k4me1StdSig.bigWig",
					"wgEncodeBroadHistoneK562H3k9acStdSig.bigWig", "wgEncodeBroadHistoneK562H3k27acStdSig.bigWig"), sep="")#	"", "")  #bw.files[["h3k4me3"]] <- load.bigWig(histone.files[1])#histone.h3k4me3 <- #bw.files[["h3k4me1"]] <- load.bigWig(histone.files[2])#histone.h3k4me1 <- #bw.files[["h3k9ac"]]  <- load.bigWig(histone.files[3])#histone.H3k9ac  <- #bw.files[["h3k27ac"]] <- load.bigWig(histone.files[4])#histone.H3k27ac <- 					

histone.h3k4me3.model <- opt.nnet(datapath= histone.files[1], rand.train.bed, rand.test.bed, rand.train.classes, main="H3K4me3")
histone.H3k4me1.model <- opt.nnet(datapath= histone.files[2], rand.train.bed, rand.test.bed, rand.train.classes, main="H3K4me1")
histone.H3k9ac.model  <- opt.nnet(datapath= histone.files[3], rand.train.bed, rand.test.bed, rand.train.classes, main="H3K9ac")
histone.H3k27ac.model <- opt.nnet(datapath= histone.files[4], rand.train.bed, rand.test.bed, rand.train.classes, main="H3K27ac")

## DNAse-1.
dnase1.path <- "/usr/data/GROseq.parser/hg19/k562/dnase/wgEncodeUwDgfK562Sig.bigWig"
dnase1.model <- opt.nnet(datapath= dnase1.path, rand.train.bed, rand.test.bed, rand.train.classes, main="DNAse-1")

dev.off()

save.image("tss.models.RData")

###################################
## Combine nnet models and evaluates performance ...

## Nerual network, of basepair resolution data in surrounding region.
## Learn from: 
#require(nnet) ## Fit single-hidden-layer neural network, possibly with skip-layer connections.
#n <- nnet(y= classes.bin, x= rbind(bw.pos.data[[1]], bw.neg.data[[1]]), size=100, MaxNWts= 15000) ## size -> num. nodes in the hidden layer.
#predict(n, rbind(bw.pos.data[[1]], bw.neg.data[[1]]))

