program: test1.o libmem.o allocators.o allocators.o bu_allocator.o
	gcc -o program -g test1.o libmem.o allocators.o bu_allocator.o

test1.o: test1.c libmem.h
	gcc -c test1.c

libmem.o: libmem.c libmem.h
	gcc -c libmem.c

allocators.o: allocators.c libmem.h
	gcc -c allocators.c

bu_allocator.o: bu_allocator.c libmem.h
	gcc -c bu_allocator.c

