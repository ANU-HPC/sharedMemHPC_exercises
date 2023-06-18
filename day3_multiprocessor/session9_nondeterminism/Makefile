SHELL = /bin/bash
CC 	= gcc
PLIBS = -lpthread
CFLAGS	= -O3 
LDFLAGS	= -lm

LDFLAGS_MPI	= -lm -lmpi

all:	p_sum

p_sum: p_sum.c
	$(CC) $(CFLAGS) -o $@ $? $(PLIBS)

.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	/bin/rm -f a.out *.o *~ p_sum

