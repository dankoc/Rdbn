#ifndef rdbn_random_H 
#define rdbn_random_H

#include <R.h>

/* Fast random number generation... */
static unsigned long x_rand=123456789,y_rand=362436069,z_rand=521288629,w_rand=88675123,v_rand=886756453;
static /*R_INLINE*/ unsigned long xorshift(void) {
 unsigned long t;
 t=(x_rand^(x_rand>>7)); x_rand=y_rand; y_rand=z_rand; z_rand=w_rand; w_rand=v_rand;
 v_rand=(v_rand^(v_rand<<6))^(t^(t<<13));

 return (y_rand+y_rand+1)*v_rand;
}

static unsigned int g_seed= 56465;
static unsigned int fastrand() {
  g_seed = (214013*g_seed+2531011);
  return (g_seed>>16)&0x7FFF;
}

#endif
