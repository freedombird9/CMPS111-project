program: test1.o libmem.o allocators.o
	gcc -o program test1.o libmem.o allocators.o

test1.o: test1.c libmem.h
	gcc -c test1.c

libmem.o: libmem.c libmem.h
	gcc -c libmem.c

allocators.o: allocators.c libmem.h
	gcc -c allocators.c
