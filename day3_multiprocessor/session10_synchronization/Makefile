CC 		= gcc
CFLAGS	= -Wall -O 

LDFLAGS	= -lpthread

all:	race bufferMon

race:	race.o
		$(CC) $(CFLAGS) -o $@ $?  $(LDFLAGS)

bufferMon:	bufferMon.o
		$(CC) $(CFLAGS) -o $@ $?  $(LDFLAGS)

.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	/bin/rm -f a.out *.o *~ race bufferMon bufferSem
