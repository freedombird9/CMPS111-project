#include <stdio.h>
#include <math.h>
#include"libmem.h"




int main(void)
{

  int i;
  int handle;
  for (i=0; i != 4; i++){

    switch(i){
    case 0:
      handle = meminit(1024, 0x4 | 0x0, 10);
      break;
    case 1:
      handle = meminit(1024, 0x4 | 0x8, 10);
      break;
    case 2:
      handle = meminit(1024, 0x4 | 0x10, 10);
      break;
    case 3:
      handle = meminit(1024, 0x4 | 0x20, 10);
      break;
    default:
      break;
    }

    int *a1 = memalloc(handle, 252);
    int *a2 = memalloc(handle, 256);
    int *a3 = memalloc(handle, 128);
    int *a4 = memalloc(handle, 64);
    int *a5 = memalloc(handle, 60);
    int *a6 = memalloc(handle, 48);

    memfree(a2);
    memfree(a4);
    memfree(a6);

    a2 = memalloc(handle, 28);
    a4 = memalloc(handle, 32);
    a6 = memalloc(handle, 36);
    int *a7 = memalloc(handle, 14);

    memfree(a6);

    a6 = memalloc(handle, 16);
    int *a8 = memalloc(handle, 128);
    int *a9 = memalloc(handle, 16);
    int *a10 = memalloc(handle, 32);
    int *a11 = memalloc(handle, 28);

    memfree(a11);
    memfree(a8);

    a8 = memalloc(handle, 70);
    a11 = memalloc(handle, 14);
    int *a12 = memalloc(handle, 20);
    int *a13 = memalloc(handle, 32);
    int *a14 = memalloc(handle, 8);

    switch(i){
    case 0:
      printf("\n");
      printf("Results for first fit:\n");
      break;
    case 1:
      printf("\n");
      printf("Results for next fit:\n");
      break;
    case 2:
      printf("\n");
      printf("Results for best fit:\n");
      break;
    case 3:
      printf("\n");
      printf("Results for worst fit:\n");
      break;
    default:
      break;
    }
    evaluate(handle);
  }
  printf("\n");
  printf("test results for random fit:\n");
  for (i = 0; i != 10; i++){
    handle = meminit(1024, 0x4 | 0x40, 10);
    int *a1 = memalloc(handle, 252);
    int *a2 = memalloc(handle, 256);
    int *a3 = memalloc(handle, 128);
    int *a4 = memalloc(handle, 64);
    int *a5 = memalloc(handle, 60);
    int *a6 = memalloc(handle, 48);

    memfree(a2);
    memfree(a4);
    memfree(a6);

    a2 = memalloc(handle, 28);
    a4 = memalloc(handle, 32);
    a6 = memalloc(handle, 36);
    int *a7 = memalloc(handle, 14);

    memfree(a6);

    a6 = memalloc(handle, 16);
    int *a8 = memalloc(handle, 128);
    int *a9 = memalloc(handle, 16);
    int *a10 = memalloc(handle, 32);
    int *a11 = memalloc(handle, 28);

    memfree(a11);
    memfree(a8);

    a8 = memalloc(handle, 70);
    a11 = memalloc(handle, 14);
    int *a12 = memalloc(handle, 20);
    int *a13 = memalloc(handle, 32);
    int *a14 = memalloc(handle, 8);

    evaluate(handle);
    printf("\n");
    printf("\n");
  }
    return 0;
}
