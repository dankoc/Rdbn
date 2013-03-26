##
##
require(Rdbn)
read_genomic_data(c(1000), rep(1, 2000), rep(1, 2000), 
		as_matrix=FALSE, zoom= list(as.integer(c(10)), as.integer(c(2))))
