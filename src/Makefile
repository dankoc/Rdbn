CC=gcc
LIBS:=-lm -lz -pthread ${shell pkg-config openssl --libs}
CFLAGS:=-c -g -Wall ${LIBS}
SOURCES= rbm.c rbm.train.c dbn.c dbn.backpropagation.c dbn.compute.c matrix_functions.c RegisterRRoutines.c
OBJECTS=$(SOURCES:.c=.o)

# define a space
empty:=
space:= $(empty) $(empty)
MFLGSESC := $(subst $(space),\$(space),\$(space))

Rdbn.so:  
	MAKEFLAGS=${MFLGSESC} R CMD SHLIB -o $@ ${SOURCES} ${CFLAGS}

clean:
	rm -f *.o *.so

