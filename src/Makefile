#ifeq ($(BIGWIGLIBHOME),)
#        BIGWIGLIBHOME := ~/bin/bigwig/bigwiglib
#endif
#
#ifeq (${MACHTYPE},)
#        MACHTYPE=x86_64
#endif

CC=gcc
LIBS:=-lm -lz -pthread ${shell pkg-config openssl --libs}
CFLAGS:=-c -g -Wall ${LIBS}
SOURCES= common_functions.c compute_priors.c \
	 update_parameters.c estimate_tf_binding.c RegisterRRoutines.c
OBJECTS=$(SOURCES:.c=.o)

#MFLGS="CFLAGS=-I/home/dankoc/bin/bigwig/bigwiglib CFLAGS=-I/home/dankoc/bin/bigwig/bigwiglib/kent/src/inc ${CFLAGS}"
MFLGS := "CFLAGS=-I/home/dankoc/bin/bigwig/bigwiglib -I/home/dankoc/bin/bigwig/bigwiglib/kent/src/inc ${CFLAGS}"
# define a space
empty:=
space:= $(empty) $(empty)
MFLGSESC := $(subst $(space),\$(space),$(MFLGS))

#.c.o:
#	$(CC) $(CFLAGS) $< -o $@

atif.so:  
	MAKEFLAGS=${MFLGSESC} R CMD SHLIB -o $@ ${SOURCES} /home/dankoc/bin/bigwig/bigwiglib/bigwiglib.a ${LIBS}

clean:
	rm -f *.o *.so

