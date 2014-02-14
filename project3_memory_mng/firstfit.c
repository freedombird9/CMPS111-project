#include <stdio.h>
#include <stdlib.h>
#include "libmem.h"

void *ff_allot(struct handle *handlers, int handleCount, long n_bytes){
  struct fl_node *search = handlers[handleCount].freelist;
  while (handlers[handleCount].freelist != NULL){
    struct fl_node *node;
    
    if (handlers[handleCount].freelist->size >= n_bytes && !handlers[handleCount].freelist->used){
      void *memAllocd = handlers[handleCount].freelist->blockstart;

      node = handlers[handleCount].freelist->blockstart + n_bytes;  /* start address of the next page */
      node->size = handlers[handleCount].freelist->size - n_bytes;
      node->used = 0;
      node->blockstart = node + sizeof(struct fl_node); /* the start address of this page's allocatable memory */

      handlers[handleCount].freelist->used = 1;          /* update old page's info */
      handlers[handleCount].freelist->size = n_bytes;
      handlers[handleCount].freelist->next = node;


      return memAllocd;
    }


  }



}
