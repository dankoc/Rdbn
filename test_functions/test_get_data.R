##
##
read_genomic_data <- function(center, plus, minus, as_matrix= TRUE, 
		zoom= list(as.integer(c(10, 25)), as.integer(c(2, 10)))) {
   .Call("get_genomic_data_R", as.integer(center), as.integer(plus), as.integer(minus), zoom, PACKAGE= "Rdbn")
}


require(Rdbn)
read_genomic_data(center= c(1000, 1025), plus= rep(1, 2000), minus= rep(0, 2000), as_matrix=TRUE, zoom= list(as.integer(c(10, 25)), as.integer(c(2, 10))))

read_genomic_data(center= c(1000, 1025), plus= rep(1, 2000), minus= rep(3, 2000), as_matrix=TRUE, zoom= list(as.integer(c(10, 25)), as.integer(c(2, 10))))
