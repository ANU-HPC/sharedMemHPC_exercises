CC = cc
CFLAGS	= 
LDFLAGS	= -lm -lpthread

all:	pi

pi:	pi.o
		$(CC) $(CFLAGS) -o $@ $?  $(LDFLAGS)

.SUFFIXES:	.o .c

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	/bin/rm -f a.out *.o *~ pi
