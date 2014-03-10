#include <lib.h>
#include <unistd.h>
#include <stdio.h>

PUBLIC set_key(k0, k1)
int k0;
int k1;

{
    message m;

    m.m1_i1 = k0;
    m.m1_i2 = k1;

    printf("handler k0 = %x k1=%x\n",k0,k1);

    printf("blabla %x\n",m.m1_i1);
    return (_syscall(FS,SET_KEY,&m));
}
