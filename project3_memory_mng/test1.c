#include "stdio.h"
#include "math.h"

int meminit(long n_bytes, unsigned int flags, int parm1);
void* memalloc(int handle, long n_bytes);
void memfree(void* region);

int main(void)
{
    int handle = meminit(1024, 0x1,8);


    void* q1 = memalloc(handle,256);
    printf("returned %p\n",q1);
    void* q2 = memalloc(handle,256);
    printf("returned %p\n",q2);
    void* q3 = memalloc(handle,512);
    printf("returned %p\n",q3);

    /*int* q2 = memalloc(handle, 252);
    int* q3 = memalloc(handle, 124);
    int* q4 = memalloc(handle, 64);
    int* q5 = memalloc(handle, 60);
    int* q6 = memalloc(handle, 60);*/

    /*memfree(q2);
    memfree(q4);
    memfree(q6);*/
    /*q2 = memalloc(handle, 28);
    q4 = memalloc(handle, 28);
    q6 = memalloc(handle, 150);*/

    return 0;
}