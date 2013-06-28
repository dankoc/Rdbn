#ifeq ($(BIGWIGLIBHOME),)
#        BIGWIGLIBHOME := ~/bin/bigwig/bigwiglib
#endif
#
#ifeq (${MACHTYPE},)
#        MACHTYPE=x86_64
#endif

CC=gcc
LIBS:=-lm -lz -fopenmp ${shell pkg-config openssl --libs}
CFLAGS:=-c -g -Wall ${LIBS}
SOURCES= rbm.c rbm.train.c dbn.c dbn.backpropagation.c dbn.compute.c matrix_functions.c RegisterRRoutines.c
OBJECTS=$(SOURCES:.c=.o)

# define a space
empty:=
space:= $(empty) $(empty)
MFLGSESC := $(subst $(space),\$(space),\$(space))

Rdbn.so:  
	MAKEFLAGS=${MFLGSESC} R CMD SHLIB -o $@ ${SOURCES} ${LIBS}

clean:
	rm -f *.o *.so

