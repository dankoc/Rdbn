##
##
require(Rdbn)
read_genomic_data(c(1000, 1025), rep(1, 2000), rep(0, 2000), as_matrix=TRUE, zoom= list(as.integer(c(10, 25)), as.integer(c(2, 10))))
