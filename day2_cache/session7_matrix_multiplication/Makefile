ATLAS=/data/atlas/ATLAS
CC 	= gcc
CFLAGS	= -O2 -fno-loop-interchange -fno-strict-aliasing # -I $(ATLAS)/include
LDFLAGS	= -lm -lpapi
#LDFLAGS=-L/$(ATLAS)/build/lib -lcblas -latlas-lm -lpapi

all:	matmul1

matmul1:	matmul1.o 
	$(CC) $(CFLAGS) -o $@ matmul1.o $(LDFLAGS)

.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	/bin/rm -f a.out *.o *~ matmul1  *.s

