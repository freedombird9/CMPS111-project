#include <stdio.h>
#include "math.h"
#include"libmem.h"


int main(void)
{
    //test for buddy allot
    //block size is exactly power of 2 or close to


    /*test case for freelist allocator*/
    /*first fit*/
    int handle5 = meminit(1024, 0x4 | 0x0, 10);
    memalloc(handle5,128);
    void* q8=memalloc(handle5,124);
    memalloc(handle5,252);
    void* q9=memalloc(handle5,128);
    memfree(q8);
    memalloc(handle5,127);
    memalloc(handle5,60);
    memfree(q9);
    memalloc(handle5,200);
    memalloc(handle5,50);


    int handle6 = meminit(1024, 0x4 | 0x0, 10);
    void* q10 =memalloc(handle6,60);
    memalloc(handle6,70);
    memalloc(handle6,300);
    void* q11 = memalloc(handle6,280);      //no enough space
    memfree(q10);
    //memalloc(handle6,100);
    //memfree(q11);
    memalloc(handle6,20);
    memalloc(handle6,180);      //no enough space*/
    //memalloc(handle6,8);

    evaluate(handle5);
    evaluate(handle6);






    return 0;
}

