CC 	= gcc
CFLAGS	= -g -O3
LDFLAGS	= -lm -lpapi

all:	cholesky

cholesky:	cholesky.o 
	$(CC) $(CFLAGS) -o $@ cholesky.o $(LDFLAGS)

test_cholesky:	cholesky
	./cholesky 200

.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	/bin/rm -f a.out *.o *~ cholesky  *.s

