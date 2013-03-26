CC=gcc
LIBS:=-lm -lz -pthread ${shell pkg-config openssl --libs}
CFLAGS:=-c -g -Wall ${LIBS}
SOURCES= read_genomic_data.c RegisterRRoutines.c 
OBJECTS=$(SOURCES:.c=.o)

#MFLGS := "CFLAGS=-I/home/dankoc/bin/bigwig/bigwiglib -I/home/dankoc/bin/bigwig/bigwiglib/kent/src/inc ${CFLAGS}"
# define a space
#empty:=
#space:= $(empty) $(empty)
#MFLGSESC := $(subst $(space),\$(space))#,$(MFLGS))

Rdbn.so:  
	MAKEFLAGS= R CMD SHLIB -o $@ ${SOURCES} ${LIBS}

clean:
	rm -f *.o *.so

