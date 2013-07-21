/*
 *  RBM training spends ~50% of time generating random numbers w/ R's unif() or C's rand()
 *  functions.  This is intedned to speed up training.
 */
 
#include "random.h"

unsigned long xorshift(void) {
 unsigned long t;
 t=(x_rand^(x_rand>>7)); x_rand=y_rand; y_rand=z_rand; z_rand=w_rand; w_rand=v_rand;
 v_rand=(v_rand^(v_rand<<6))^(t^(t<<13));

 return (y_rand+y_rand+1)*v_rand;
}