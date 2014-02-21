
libmem.o: libmem.c libmem.h
	gcc -c libmem.c

freelist_allocators.o: freelist_allocators.c libmem.h
	gcc -c freelist_allocators.c

bu_allocator.o: bu_allocator.c libmem.h
	gcc -c bu_allocator.c

libmem.a: libmem.o freelist_allocators.o bu_allocator.o
	ar rcs libmem.a libmem.o freelist_allocators.o bu_allocator.o




