#
# Makefile for project4
#

#MAKEIMG = make image
#MAKEIST = make install
#MMAKE = make makefile
#MAKELIB = make libraries

FSDIR = /usr/src/servers/fs/
LIBDIR = /usr/src/lib/posix/
HEADERDIRF = /usr/src/include/minix/
HEADERDIRS = /usr/include/minix/


# copy the target files to its dir


copy:
	cp protect.c $(FSDIR)
	cp glo.h $(FSDIR)
	cp table.c $(FSDIR)
	cp proto.h $(FSDIR)
	cp const.h $(HEADERDIRF)
	cp callnr.h $(HEADERDIRF)
	cp const2.h $(HEADERDIRS)const.h
	cp callnr2.h $(HEADERDIRS)callnr.h
	cp _set_key.c $(LIBDIR)
	cp Makefile.in $(LIBDIR)
	cp read.c $(FSDIR)
	cp rijndael.c $(FSDIR)
	cp rijndael.h $(FSDIR)
	cp Makefilefs $(FSDIR)Makefile


#make
all:
	cd /usr/src/servers/ && make image
	cd /usr/src/servers/ && make install
	cd $(LIBDIR) && make Makefile
	cd /usr/src/ && make libraries
	cd /usr/src/tools && make hdboot
	cd /usr/src/tools && make install


delete:
	rm $(FSDIR)protect.c
	rm $(FSDIR)glo.h
	rm $(FSDIR)table.c
	rm $(FSDIR)proto.h
	rm $(HEADERDIRF)const.h
	rm $(HEADERDIRF)callnr.h
	rm $(HEADERDIRS)const.h
	rm $(HEADERDIRS)callnr.h
	rm $(LIBDIR)_set_key.c
	rm $(LIBDIR)Makefile.in
	rm $(FSDIR)read.c 
	rm $(FSDIR)rijndael.c
	rm $(FSDIR)rijndael.h
	rm $(FSDIR)Makefile

key:
	cc setkey.c -o setkey




