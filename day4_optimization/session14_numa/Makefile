# makefile for for CISRA Workshop Matrix Nultiply exrcises
# by Peter Strazdins, RSCS ANU, Aug 2015

CC=icc
CCFLAGS=-O3 -mkl
LDFLAGS=-lnuma  -lm 

.SUFFIXES:
.PRECIOUS: %.o

OBJS=mat.o matmult.o 

EXE=matmultest
EXE2=matmultestnuma

all: $(EXE)

%: %.o $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $*.o $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $*.c

clean:
	rm -f $(OBJS) $(EXE) $(EXE).o  $(EXE2) $(EXE).o $(EXE2).o *~
