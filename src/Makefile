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
SOURCES= common_functions.c compute_priors.c 
OBJECTS=$(SOURCES:.c=.o)

#.c.o:
#	$(CC) $(CFLAGS) $< -o $@

atif.so:  
	MAKEFLAGS=R CMD SHLIB -o $@ ${SOURCES} ${LIBS}

clean:
	rm -f *.o *.so

