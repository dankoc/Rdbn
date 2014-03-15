##
## Utility for shuffling an arbitrary number of elements based on an arbitrary number of classes.
##

## TEST:
# a <- c(rep(1, 10),rep(2,10),rep(3,10),rep(4,10),rep(5,10))
# a
# a[shuffle(50,5)]

shuffle <- function(n_elements, n_groups=2) {
  indx <- c(1:n_elements)
  shuf <- NULL
  for(i in n_groups:1) {
    shuf <- c(shuf, which(indx %% n_groups == (i-1)))
  }
  return(order(shuf))
}
