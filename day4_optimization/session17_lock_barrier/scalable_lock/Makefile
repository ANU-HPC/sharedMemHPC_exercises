CC=gcc -O 

objects = locks.o lock_user.o
locktest: $(objects)
	 $(CC) -o locktest $(objects) -lpthread	

%.s: %.c
	$(CC) -S $*.c

locks.o:locks.h simple_locks.c
locks.s: simple_locks.c
lock_user.o:locks.h

.PHONY : clean
clean:
	rm -f locktest $(objects)
