CC 		= gcc
CFLAGS	= -O3 -fopenmp
#CC	= icx
#CFLAGS	= -O3 -qopenmp
LDFLAGS	= -lm

all:	ompexample1 ompexample2 ompexample3 ompexample4 \
	ompexample5 ompexample6

ompexample1:	ompexample1.c
	$(CC) $(CFLAGS) -o $@ $? $(LDFLAGS)

ompexample2:	ompexample2.c
	$(CC) $(CFLAGS) -o $@ $? $(LDFLAGS)

ompexample3:	ompexample3.c
	$(CC) $(CFLAGS) -o $@ $? $(LDFLAGS)

ompexample4:	ompexample4.c
	$(CC) $(CFLAGS) -o $@ $? $(LDFLAGS)

ompexample5:	ompexample5.c
	$(CC) $(CFLAGS) -o $@ $? $(LDFLAGS)

ompexample6:	ompexample6.c
	$(CC) $(CFLAGS) -o $@ $? $(LDFLAGS)

.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	/bin/rm -f a.out *.o *~ ompexample[1-6]

