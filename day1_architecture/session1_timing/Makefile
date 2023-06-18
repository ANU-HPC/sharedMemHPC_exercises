ARCH = $(shell uname -m)

ifeq ($(ARCH),sun4v)
CC = cc
CFLAGS	= 
else 
CC = gcc
CFLAGS	= 
endif

PROGS=walltime linear cputime dateTime

LDFLAGS	= -lm

all: $(PROGS)	

walltime:	walltime.o
		$(CC) $(CFLAGS) -o $@ $?  $(LDFLAGS)

cputime:	cputime.o
		$(CC) $(CFLAGS) -o $@ $?  $(LDFLAGS)

linear:		linear.o
		$(CC) $(CFLAGS) -o $@ $? $(LDFLAGS)

.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	/bin/rm -f a.out *.o *~ $(PROGS)
