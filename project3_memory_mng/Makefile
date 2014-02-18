program: test1.o libmem.o freelist_allocators.o bu_allocator.o
	gcc -o program test1.o libmem.o freelist_allocators.o bu_allocator.o -lm

test1.o: test1.c libmem.h
	gcc -c test1.c

libmem.o: libmem.c libmem.h
	gcc -c libmem.c

freelist_allocators.o: freelist_allocators.c libmem.h
	gcc -c freelist_allocators.c

bu_allocator.o: bu_allocator.c libmem.h
	gcc -c bu_allocator.c
