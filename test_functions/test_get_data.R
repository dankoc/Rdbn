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
GROcap_tss_bed <- read.table("/usr/projects/GROseq.parser/tss_new/hg19.k562.new_hmm2.bed", skip=1)

indx <- 42
step_size=10
n_windows=20
chrom_t1=as.character(GROcap_tss_bed[indx,1])
cente_t1=as.integer(GROcap_tss_bed[indx,2]+(GROcap_tss_bed[indx,3]-GROcap_tss_bed[indx,2])/2)

a <- as.real(read_genomic_data(chrom_t1, cente_t1, bw_data_plus, bw_data_minus, zoom=list(window_sizes= as.integer(c(step_size)), half_n_windows= as.integer(n_windows)) ))

b <- c(collect.counts(bw_data_plus,  chrom=chrom_t1, start= cente_t1-step_size*n_windows, end= cente_t1+step_size*n_windows, step=step_size),
-1*collect.counts(bw_data_minus, chrom=chrom_t1, start= cente_t1-step_size*n_windows, end= cente_t1+step_size*n_windows, step=step_size))
a
b
