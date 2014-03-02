CC=gcc
LIBS:=-pthread
CFLAGS:=-O2 -c -g ${LIBS}
SOURCES= rbm.c rbm.train.c dbn.c dbn.backpropagation.c dbn.compute.c dbn.daydream.c dbn.clamplayer.c matrix_functions.c random.c RegisterRRoutines.c
OBJECTS=$(SOURCES:.c=.o)

# build our MAKEFLAGS
# R needs spaces to be escaped ...
#
MFLGS := "CFLAGS=$(CFLAGS)"
# define a space
empty:=
space:= $(empty) $(empty)
MFLGSESC := $(subst $(space),\$(space),$(MFLGS))

Rdbn.so: $(SOURCES)
	MAKEFLAGS=${MFLGSESC} R CMD SHLIB ${LIBS} -o $@ ${SOURCES}

clean:
	rm -f *.o *.so

