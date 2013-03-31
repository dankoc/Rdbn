##
## Test combo R/C function
#GROcap_tss_bed <- read.table("/usr/projects/GROseq.parser/tss_new/hg19.k562.new_hmm2.bed", skip=1)
require(Rdbn)

gs_plus  <- "/usr/data/GROseq.parser/hg19/k562/proseq_celastrol_prelim/celastrol_proseq_0min_plus.bigWig"
gs_minus <- "/usr/data/GROseq.parser/hg19/k562/proseq_celastrol_prelim/celastrol_proseq_0min_minus.bigWig"
bw_data_plus <- load.bigWig(gs_plus)
bw_data_minus <- load.bigWig(gs_minus)

step_size=10
n_windows=20

Gencode <- read.table("/usr/projects/GROseq.parser/annotations/gencode.comprehensive.bed", header=FALSE, skip=1)
Gencode <- Gencode[Gencode[,11]== "protein_coding",c(1:3)]
Gencode <- Gencode[sample(c(1:NROW(Gencode)), 10000),]

bigWig_gencode_data <- colSums(collect.many(Gencode, bw_data_plus, bw_data_minus, halfWindow= step_size*n_windows, step= step_size))
Rdbn_gencode_data <- colSums(read_genomic_data(Gencode, gs_plus, gs_minus, window_sizes= step_size, half_nWindows= n_windows))

plot(bigWig_gencode_data, type="l", ylim=c(0, max(c(gencode_data, Rdbn_gencode_data))))
points(Rdbn_gencode_data, type="l", col="red")
