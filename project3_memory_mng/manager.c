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

void initBitmap(int depth, struct bu_node *root){
  root->used=0;
  if(depth==0){
    root->left=NULL;
    root->right=NULL;
  }
  while(depth>0){
    depth=depth-1;
    root->left=malloc(sizeof(struct bu_node));
    root->right=malloc(sizeof(struct bu_node));
    initBitmap(depth, root->left);
    initBitmap(depth, root->right);
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
  if (flags & 0x4) {            /* initiate free list allocator */
    handlers[handleCount].freelist = malloc(sizeof(struct fl_node));
    handlers[handleCount].freelist->blockstart = handlers[handleCount].memstart;
    handlers[handleCount].freelist->size = n_bytes;
    handlers[handleCount].freelist->next = NULL;
  }

  else if (flags & 0x1){
    int depth = power - parm1 + 1;
    handlers[handleCount].bitmap=malloc(sizeof(struct bu_node));
    initBitmap(depth, handlers[handleCount].bitmap);
  }
  return handleCount++;
}

void *memalloc(int handle, long n_bytes){
    void *p;
    if(handlers[handle].flags & 0x1){
        /*call buddy allocator*/
        p=buddy_allot(handlers[handle], n_bytes);
    }
    else if(handlers[handle].flags & 0x4){
        /*determine type*/
        if(handlers[handle].flags == (0x0 | 0x4)){
            /*call first fit*/
            p=ff_allot(handlers[handle], n_bytes);
        }
        if(handlers[handle].flags == (0x8 | 0x4)){
            /*call next fit*/
            p=nf_allot(handlers[handle], n_bytes);
        }
        if(handlers[handle].flags == (0x10 | 0x4)){
            /*call best fit*/
            p=bf_allot(handlers[handle], n_bytes);
        }
        if(handlers[handle].flags == (0x20 | 0x4)){
            /*call worst fit*/
            p=wf_allot(handlers[handle], n_bytes);
        }
        if(handlers[handle].flags == (0x40 | 0x4)){
            /*call random fit*/
            p=rf_allot(handlers[handle], n_bytes);
        }
    }
    return p;
}

void memfree (void *region){

}

void print_fl(struct handle *this_handle){
    struct fl_node *walk;
    walk=this_handle->freelist;

}
