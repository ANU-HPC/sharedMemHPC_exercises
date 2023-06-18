SHELL = /bin/bash
CC 	= gcc
PLIBS = -lpthread
CFLAGS	= -O0 -g
LDFLAGS	= -lm

LDFLAGS_MPI	= -lm -lmpi

all:	p_dead

p_dead: p_dead.c
	$(CC) $(CFLAGS) -o $@ $? $(PLIBS)

.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:	
	/bin/rm -f ./p_dead *.o *~
