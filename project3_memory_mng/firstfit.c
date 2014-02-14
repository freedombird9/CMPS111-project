#include <stdio.h>
#include <stdlib.h>
#include "libmem.h"

void *ff_allot(struct handle *handlers, int handleCount, long n_bytes){
  struct fl_node *search = handlers[handleCount].freelist;

  while (handlers[handleCount].freelist != NULL){
    struct fl_node *node;
    if (search->size >= n_bytes && !search->used){
      void *memAllocd = handlers[handleCount].freelist->blockstart;

      node = handlers[handleCount].freelist->blockstart + n_bytes;  /* start address of the next page */
      node->size = handlers[handleCount].freelist->size - n_bytes;
      node->used = 0;
      node->blockstart = node + sizeof(struct fl_node); /* the start address of this page's allocatable memory */
      node->next = NULL;


      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      search->next = node;

      return memAllocd;
    }
    search = search->next;

  }

}
