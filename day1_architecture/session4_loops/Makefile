CC 	= gcc
CFALIAS	= -O2 -funroll-loops 
CFLAGS	= $(CFALIAS) -fno-strict-aliasing
CFLAGS_MAT = -O3
LDFLAGS	= -lm -lpapi

all:	matvec1 matvec2 

matvec1:	matvec1.o myvec.o
	$(CC) $(CFLAGS_MAT) -o $@ matvec1.o myvec.o $(LDFLAGS)

matvec2:	matvec2.o myvec.o
	$(CC) $(CFLAGS_MAT) -o $@ matvec2.o myvec.o $(LDFLAGS) 


.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS_MAT) -c $<

clean:
	/bin/rm -f a.out *.o *~ matvec[1-2]  *.s

