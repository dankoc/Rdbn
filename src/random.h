#ifndef rdbn_random_H 
#define rdbn_random_H

/* Fast random number generation... */
static global unsigned long x_rand=123456789,y_rand=362436069,z_rand=521288629,w_rand=88675123,v_rand=886756453;
unsigned long xorshift(void);

#endif