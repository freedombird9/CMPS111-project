#include "stdio.h"
#include "libmem.h"

int meminit(long n_bytes, unsigned int flags, int parm1);
void* memalloc(int handle, long n_bytes);
void memfree(void* region);
int power(int num);


int main (void){
    int i;
    for (i=0;i<10; i++){
        printf("%d, %d\n",i,(int)power(i));
    }

}
