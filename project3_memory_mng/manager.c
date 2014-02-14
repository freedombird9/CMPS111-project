#include <stdio.h>
#include <stdlib.h>
#include "libmem.h"

int isValid(long number){
  int power;
  while(number % 2 == 0){
    number /= 2;
    power++;
    if (number == 1)
      return power;
  }
  return 0;
}

void initBitmap(int depth, struct bu_node **root){
  root->used=0;
  if(depth==0){
    root->left=NULL;
    root->right=NULL;
  }
  while(depth){
    depth=depth-1;
    initBitmap(depth, &root->left);
    initBitmap(depth, &root->right);
  }
}



int meminit(long n_bytes, unsigned int flags, int parm1){
  int i, j;
  int power = isValid(n_bytes);
  if (power)
    return -1;
  handlers[handleCount].memstart = malloc(n_bytes);
  handlers[handleCount].flags = flags;
  handlers[handleCount].n_bytes = n_bytes;

  /* free list allocator */
  if (flags == 0x4) {            /* initiate free list allocator */
    handlers[handleCount].freelist = malloc(sizeof(struct fl_node));
    handlers[handleCount].freelist->blockstart = handlers[handleCount].memstart;
    handlers[handleCount].freelist->size = n_bytes;
  }

  else if (flags == 0x1){
    int depth = power - parm1 + 1;
    initBitmap(depth, &handlers[handleCount].bitmap);
  }
  
  return handleCount++;
}

void *memalloc
