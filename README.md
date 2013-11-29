Rdbn
====
Learn deep belief networks in R.

Rdbn was written to allow training and classification using restricted Boltzmann machines (RBMs) and deep belief networks (DBNs) in an R environment.  

Rdbn supports:
* Pre-training a deep belief network using ideas from 'contrastive divergence'.
* Fine-tuning the network for classification tasks using the backpropagation algorithm.
* Advanced training features such as momentum-accelerated learning, and L2 regularization.
* Parallel processing on UNIX-based systems using pthreads.

Rdbn was written quickly (for educational reasons), and is still actively undergoing testing and optimization.  When I'm confident that the package is reasonably bug-free, it will be submitted to CRAN (possibly being partially re-written to incorporate faster linear algebra libraries).

Installing
----------
In a UNIX based OS with R and dev tools installed, one can simply type: 

    R CMD install Rdbn/

R should take care of the rest.  Note that on OS X developer tools must be installed in order to have a C compiler.  Windows -should- work without the parallel processing advantages, but has not yet been tested.


Using the package
-----------------
To train a classifier on the Vehicle dataset in ML bench, use the following:

    require(mlbench)
    data(Vehicle)
    require(Rdbn)
    set.seed(34) ## Different starting points can result in different performance.

    ###
    ## Transform covariates for optimal classification by a neural network.
    x <- t(Vehicle[,c(1:18)])
    y <- Vehicle[,19]
    for(i in c(1:(NCOL(Vehicle)-1))) {
      x[i,] <- logistic_function(scale(Vehicle[,i])*2) ## 2 is an arbitrarily chosen value... 
    }

    ###
    ## Divide the data into training and test sets.
    trainIndx <- sample(c(1:NCOL(x)), NCOL(x)*0.8, replace=FALSE)
    testIndx <- c(1:NCOL(x))[!(c(1:NCOL(x)) %in% trainIndx)]

    ###
    ## Quick & Simple calls to train a classifier using deep belief networks.
    db <- dbn(x= x[,trainIndx], y= y[trainIndx], layer_sizes= c(18,100,150), 
      batch_size=10, momentum_decay= 0.9, learning_rate=0.1, weight_cost= 0.01, n_threads=8)
    pred_dbn <- dbn.predict(db, data=x[,testIndx], n_threads=8)

    print(paste("% correct (dbn): ", sum(pred_dbn == as.character(y[testIndx]))/NROW(y[testIndx])))

    ###
    ## Alternatively, network training strategies can be applied independently.  
    ## This provides additional control over training parameters, and can result in better performance.
    db <- dbn(layer_sizes= c(18,100,150), batch_size=10, cd_n=1, momentum_decay= 0.9, 
      learning_rate=0.1, weight_cost= 0.01)
    db <- dbn.pretrain(db, data= x[,trainIndx], n_epocs= 50, n_threads=8)

    ## Refine using backpropagation with new learning parameters.
    db_refine <- dbn.refine(db, data= x[,trainIndx], labels= y[trainIndx], n_epocs=100, rate_mult=10, n_threads=8)
    pred_dbn <- dbn.predict(db_refine, data=x[,testIndx], n_threads=8)

    print(paste("% correct (dbn): ", sum(pred_dbn == as.character(y[testIndx]))/NROW(y[testIndx])))

See files in the 'Rdbn/test_functions/' folder for additional examples of how to interact with the package in R.  Vignette and reference manual coming soon!

Tips for Designing a Useful Network
-----------------------------------
When training a network, it is important that each mini-batch contains a mixed set of examples from each of the class(es) that the network is designed to recognize.  Rdbn does *not* make any changes to the order of the training examples.  

If all examples of a particular class appear together in the training sequence, it is very important to permute the training examples before passing to Rdbn.  The following function, shuffle, returns a permuted order that mixes up different sides of the vector.  This function works for a two-class case, but the idea can be extended to additional classes: 

    ## Shuffles the last 50% of elements into the 1st 50%.
    shuffle <- function(n_elements) {
      indx <- c(1:n_elements)
      shuf <- c(which(indx %% 2 == 1), which(indx %% 2 == 0))
      return(order(shuf))
    }

Implementation Notes
--------------------
* In the future I may re-write to use vector form and optimized libraries (i.e., BLAS or LAPAK).  This will speed up training large networks.
* I'm also interested in adding CUDA support ... so much to do, so little time.
* Rdbn -should- compile on Windows at this point.  If you get it working, please drop me a note.

Useful References
-----------------
* Nice Science paper demonstrating some useful applications: http://www.cs.toronto.edu/~hinton/science.pdf
* General overview on Scholarpedia (by Hinton): http://www.scholarpedia.org/article/Boltzmann_machine and http://www.scholarpedia.org/article/Deep_belief_networks
* Some excellent, abridged notes on the 'dark art' parts of the method: http://www.cs.toronto.edu/~hinton/absps/guideTR.pdf
* Hinton's Coursea course on neural networks: https://class.coursera.org/neuralnets-2012-001/lecture/index
* Several additional references listed in specific sections of the source code.

License
=======

Copyright 2013 Charles Danko

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 
   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
