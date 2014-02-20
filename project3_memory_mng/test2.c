#include <stdio.h>
#include "math.h"
#include "libmem.h"


int main(void)
{
    //test for buddy allot
    //block size is exactly power of 2 or close to
    int handle = meminit(1024, 0x1,6);

    memalloc(handle,128);
    void* q2=memalloc(handle,124);
    memalloc(handle,252);
    void* q3=memalloc(handle,128);
    memfree(q2);
    memalloc(handle,127);
    memalloc(handle,60);
    memfree(q3);
    memalloc(handle,200);
    memalloc(handle,50);


    int handle2 = meminit(1024, 0x1,7);

    memalloc(handle2,128);
    void* q4=memalloc(handle2,124);
    memalloc(handle2,252);
    void* q5=memalloc(handle2,128);
    memfree(q4);
    memalloc(handle2,127);
    memalloc(handle2,60);
    memfree(q5);
    memalloc(handle2,200);
    memalloc(handle2,50);


    int handle3 = meminit(1024, 0x1,6);

    void* q6 =memalloc(handle3,60);
    memalloc(handle3,70);
    memalloc(handle3,300);
    void* q7 = memalloc(handle3,280);
    memfree(q6);
    memalloc(handle3,100);
    memfree(q7);
    memalloc(handle3,20);
    memalloc(handle3,180);
    memalloc(handle3,8);


    /*int handle4 = meminit(1024, 0x1,8);

    void* q13 = memalloc(handle4,60);
    printf("returned %p\n",q13);
    void* q14 = memalloc(handle4,70);
    printf("returned %p\n",q14);
    void* q15 = memalloc(handle4,280);
    printf("returned %p\n",q15);
    memfree(q14);
    void* q16= memalloc(handle4,100);
    printf("returned %p\n",q16);*/

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


    status(handle);
    status(handle2);
    status(handle3);
    evaluate(handle5);
    //evaluate(handle6);






    return 0;
}
