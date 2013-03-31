##
##
read_genomic_data_c <- function(center, plus, minus, as_matrix= TRUE, 
		zoom= list(as.integer(c(10, 25)), as.integer(c(2, 10)))) {
   .Call("get_genomic_data_R", as.integer(center), as.integer(plus), as.integer(minus), zoom, PACKAGE= "Rdbn")
}


require(Rdbn)
read_genomic_data_c(center= c(1000, 1025), plus= rep(1, 2000), minus= rep(0, 2000), as_matrix=TRUE, zoom= list(as.integer(c(10, 25)), as.integer(c(2, 10))))

read_genomic_data_c(center= c(1000, 1025), plus= rep(1, 2000), minus= rep(3, 2000), as_matrix=TRUE, zoom= list(as.integer(c(10, 25)), as.integer(c(2, 10))))


##
## Test combo R/C function
require(Rdbn)
GROcap_tss_bed <- read.table("/usr/projects/GROseq.parser/tss_new/hg19.k562.new_hmm2.bed", skip=1)
gs_plus  <- "/usr/data/GROseq.parser/hg19/k562/proseq_celastrol_prelim/celastrol_proseq_0min_plus.bigWig"
gs_minus <- "/usr/data/GROseq.parser/hg19/k562/proseq_celastrol_prelim/celastrol_proseq_0min_minus.bigWig"
bw_data_plus <- load.bigWig(gs_plus)
bw_data_minus <- load.bigWig(gs_minus)

indx <- 42012
step_size=10
n_windows=20
chrom_t1=as.character(GROcap_tss_bed[indx,1])
cente_t1=as.integer(GROcap_tss_bed[indx,2]+(GROcap_tss_bed[indx,3]-GROcap_tss_bed[indx,2])/2)

a <- as.real(read_genomic_data(chrom_t1, cente_t1, bw_data_plus, bw_data_minus, zoom=list(window_sizes= as.integer(c(step_size)), half_n_windows= as.integer(n_windows)) ))

b <- c(collect.counts(bw_data_plus,  chrom=chrom_t1, start= cente_t1-step_size*n_windows, end= cente_t1+step_size*n_windows, step=step_size),
-1*collect.counts(bw_data_minus, chrom=chrom_t1, start= cente_t1-step_size*n_windows, end= cente_t1+step_size*n_windows, step=step_size))

collect.counts(bw_data_plus,  chrom=chrom_t1, start= cente_t1-step_size*n_windows, end= cente_t1+step_size*n_windows, step=step_size)
collect.counts(bw_data_minus, chrom=chrom_t1, start= cente_t1-step_size*n_windows, end= cente_t1+step_size*n_windows, step=step_size)

##
## Test a background set ... do we really have peaks in the background?!

Gencode <- read.table("/usr/projects/GROseq.parser/annotations/gencode.comprehensive.bed", header=FALSE, skip=1)
Gencode <- Gencode[Gencode[,11]== "protein_coding",c(1:3)]
Gencode <- Gencode[sample(c(1:NROW(Gencode)), 10000),]

bigWig_gencode_data <- colSums(collect.many(Gencode, bw_data_plus, bw_data_minus, halfWindow= step_size*n_windows, step= step_size))

Rdbn_gencode_data <- colSums(read_genomic_data(Gencode[,1], (Gencode[,3]+Gencode[,2])/2, bw_data_plus, bw_data_minus, zoom=list(window_sizes= as.integer(c(step_size)), half_n_windows= as.integer(n_windows)) ))

plot(bigWig_gencode_data, type="l", ylim=c(0, max(c(gencode_data, Rdbn_gencode_data))))
points(Rdbn_gencode_data, type="l", col="red")
