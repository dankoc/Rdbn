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
SOURCES= read_genomic_data.c RegisterRRoutines.c
OBJECTS=$(SOURCES:.c=.o)

MFLGS := "CFLAGS=-I/home/dankoc/bin/bigwig/bigwiglib -I/home/dankoc/bin/bigwig/bigwiglib/kent/src/inc ${CFLAGS}"
# define a space
empty:=
space:= $(empty) $(empty)
MFLGSESC := $(subst $(space),\$(space),$(MFLGS))

#.c.o:
#	$(CC) $(CFLAGS) $< -o $@

Rdbn.so:  
	MAKEFLAGS=${MFLGSESC} R CMD SHLIB -o $@ ${SOURCES} /home/dankoc/bin/bigwig/bigwiglib/bigwiglib.a ${LIBS}

clean:
	rm -f *.o *.so

