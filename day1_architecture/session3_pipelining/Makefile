HOST=$(shell hostname | awk -F- '{print $$1}')
CC 	= gcc
##CFLAGS	= $(CFALIAS) -fno-strict-aliasing
#CFALIAS	= -O3 -funroll-loops 
#CFLAGS= -O2 -funroll-loops
CFLAGS_MAT= -O3
LDFLAGS= -lm 

all:	vadd.s runadd1 runadd2 runadd1_perf runadd2_perf

vadd.s:	vadd.c
	$(CC) -O2 -funroll-loops  -S vadd.c

runadd1:	runadd.o
ifeq ($(HOST),gadi)
	$(CC) -O2 -funroll-loops -o $@ runadd.o vadd.c $(LDFLAGS)
else
	$(CC) -O1 -funroll-loops -o $@ runadd.o vadd.c $(LDFLAGS)
endif

runadd2:	runadd.o
ifeq ($(HOST),gadi)
	$(CC) -O3 -funroll-loops -o $@ runadd.o vadd.c $(LDFLAGS)
else
	$(CC) -O2 -funroll-loops -o $@ runadd.o vadd.c $(LDFLAGS)
endif

runadd1_perf:	runadd_perf.o
ifeq ($(HOST),gadi)
	$(CC) -O2 -funroll-loops -o $@ runadd_perf.o vadd.c $(LDFLAGS) -lpapi
else
	$(CC) -O1 -funroll-loops -o $@ runadd_perf.o vadd.c $(LDFLAGS) -lpapi
endif

runadd2_perf:	runadd_perf.o
ifeq ($(HOST),gadi)
	$(CC) -O3 -funroll-loops -o $@ runadd_perf.o vadd.c $(LDFLAGS) -lpapi
else
	$(CC) -O2 -funroll-loops -o $@ runadd_perf.o vadd.c $(LDFLAGS) -lpapi
endif

.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS_MAT) -c $<

clean:
	/bin/rm -f a.out *.o *~ *.lst runadd[1-2] runadd[1-2]_perf *.s *.lst

