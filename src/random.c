/*
 *  RBM training spends ~50% of time generating random numbers w/ R's unif() or C's rand()
 *  functions.  This is intedned to speed up training.
 */
 
#include "random.h"

unsigned long xorshift(void) {
 unsigned long t;
 t=(x^(x>>7)); x=y; y=z; z=w; w=v;
 v=(v^(v<<6))^(t^(t<<13));

 return (y+y+1)*v;
}