CC=gcc
LIBS:=-lm -lz -pthread
CFLAGS:=-c -g -Wall ${LIBS}
SOURCES= rbm.c rbm.train.c dbn.c dbn.backpropagation.c dbn.compute.c dbn.daydream.c matrix_functions.c random.c RegisterRRoutines.c
OBJECTS=$(SOURCES:.c=.o)

# define a space
empty:=
space:= $(empty) $(empty)
MFLGSESC := $(subst $(space),\$(space),\$(space))

Rdbn.so:  
	MAKEFLAGS=${MFLGSESC} R CMD SHLIB -o $@ ${SOURCES} ${CFLAGS}

clean:
	rm -f *.o *.so

