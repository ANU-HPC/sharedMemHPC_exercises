CC=gcc -O

objects = barriers.o test_barriers.o
test_barriers: $(objects)
	$(CC) -o test_barriers $(objects) -lpthread	

%.s: %.c
	$(CC) -S $*.c


barriers.o: barriers.h
test_barriers.o: barriers.h

.PHONY : clean
clean:
	rm -f test_barriers $(objects)
