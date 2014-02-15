#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "libmem.h"


/* first fit allocator */
void *ff_allot(struct handle *handlers, int handleCount, long n_bytes){
  struct fl_node *search = handlers[handleCount].freelist;

  while (search != NULL){
    struct fl_node *node;
    if (search->size > n_bytes && !search->used){
      char *memAllocd = search->blockstart;
      
      if (search->size < n_bytes + sizeof(struct fl_node)){  /* if there's no room for the new overhead after allocating n_bytes */
	search->used = 1;                                    /* just allocate all the available memory */
	(handlers[handleCount].numNodes)--;    /* one free node used */
	return (void*) memAllocd;
      }
      node = search->blockstart + n_bytes;  /* start address of the next node */
      node->size = search->size - n_bytes - sizeof(struct fl_node);
      node->used = 0;
      node->blockstart = ((char*) node) + sizeof(struct fl_node); /* the start address of this page's allocatable memory */
      node->next = search->next;
      
      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      search->next = node;
      
      return (void*) memAllocd;
    }
    if (search->size == n_bytes && !search->used){   /* if it fits exactly into the free memory, a special case */
      char *memAllocd = search->blockstart;

      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      (handlers[handleCount].numNodes)--;    /* one free node used */

      return (void*) memAllocd;
    }
    search = search->next;
  }
  return NULL;
}

/* next fit allocator */
void *nf_allot(struct handle *handlers, int handleCount, long n_bytes){    
  struct fl_node *search = handlers[handleCount].visited;
  search = search->next;       /* start searching from the next node */
  while (search != NULL){
    struct fl_node *node;
    if (search->size > n_bytes && !search->used){
      char *memAllocd = search->blockstart;

      if (search->size < n_bytes + sizeof(struct fl_node)){  /* if there's no room for the new overhead after allocating n_bytes */
	search->used = 1;                                    /* just allocate all the available memory */
	(handlers[handleCount].numNodes)--;    /* one free node used */
	return (void*) memAllocd;
      }
      
      node = search->blockstart + n_bytes;  /* start address of the next node */
      node->size = search->size - n_bytes - sizeof(struct fl_node);
      node->used = 0;
      node->blockstart = ((char*) node) + sizeof(struct fl_node); /* the start address of this page's allocatable memory */
      node->next = search->next;

      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      search->next = node;

      handlers[handleCount].visited = search;

      return (void*) memAllocd;
    }
    
    if (search->size == n_bytes && !search->used){   /* if it fits exactly into the free memory, a special case */
      char *memAllocd = search->blockstart;
      
      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      (handlers[handleCount].numNodes)--;    /* one free node used */

      handlers[handleCount].visited = search;
      
      return (void*) memAllocd;
   }
    search = search ->next;
  }
  return NULL;
}

/* best fit allocator */

void bf_allot(struct handle *handlers, int handleCount, long n_bytes){
   struct fl_node *search = handlers[handleCount].freelist;
   struct fl_node *best = handlers[handleCount].freelist;
   
   while (search != NULL){
     if (search->size >= n_bytes && search->size < best->size){ /* update the best fit node */
       best = search;
     }
   }

  while (search != NULL){
    struct fl_node *node;
    if (search->size > n_bytes && !search->used){
      char *memAllocd = search->blockstart;

      if (search->size < n_bytes + sizeof(struct fl_node)){  /* if there's no room for the new overhead after allocating n_bytes */
	search->used = 1;                                    /* just allocate all the available memory */
	(handlers[handleCount].numNodes)--;    /* one free node used */
	return (void*) memAllocd;
      }
      
      node = search->blockstart + n_bytes;  /* start address of the next node */
      node->size = search->size - n_bytes - sizeof(struct fl_node);
      node->used = 0;
      node->blockstart = ((char*) node) + sizeof(struct fl_node); /* the start address of this page's allocatable memory */
      node->next = search->next;
      
      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      search->next = node;
      
      return (void*) memAllocd;
    }
    if (search->size == n_bytes && !search->used){   /* if it fits exactly into the free memory, a special case */
      char *memAllocd = search->blockstart;
      
      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      (handlers[handleCount].numNodes)--;    /* one free node used */
      
      return (void*) memAllocd;
    }
    search = search->next;
  }
  return NULL;
  
}


/* worst fit allocator */

void wf_allot(struct handle *handlers, int handleCount, long n_bytes){
   struct fl_node *search = handlers[handleCount].freelist;
   struct fl_node *worst = handlers[handleCount].freelist;
   
   while (search != NULL){
     if (search->size >= n_bytes && search->size > best->size){ /* update the best fit node */
       worst = search;
     }
   }

  while (search != NULL){
    struct fl_node *node;
    if (search->size > n_bytes && !search->used){
      char *memAllocd = search->blockstart;

      if (search->size < n_bytes + sizeof(struct fl_node)){  /* if there's no room for the new page after allocating n_bytes */
	search->used = 1;                                    /* just allocate all the available memory */
	(handlers[handleCount].numNodes)--;    /* one free node used */
	return (void*) memAllocd;
      }
      
      node = search->blockstart + n_bytes;  /* start address of the next node */
      node->size = search->size - n_bytes - sizeof(struct fl_node);
      node->used = 0;
      node->blockstart = ((char*) node) + sizeof(struct fl_node); /* the start address of this page's allocatable memory */
      node->next = search->next;
      
      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      search->next = node;
      
      return (void*) memAllocd;
    }
    if (search->size == n_bytes && !search->used){   /* if it fits exactly into the free memory, a special case */
      char *memAllocd = search->blockstart;
      
      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      (handlers[handleCount].numNodes)--;    /* one free node used */
      
      return (void*) memAllocd;
    }
    search = search->next;
  }
  return NULL;
}

/* random fit allocator */

void rf_allot(struct handle *handlers, int handleCount, long n_bytes){
  srand(time(NULL));      /* seed the random number generator */
  struct fl_node *search = handlers[handleCount].freelist;
  unsigned int lucky_num = rand() % handlers[handleCount].numNodes;  /* use a lucky number to choose the nodes */
  
  while (search != NULL){
    struct fl_node *node;
    
    if (search->size >= n_bytes && !search->used)
      lucky_num--;
    if (search->size > n_bytes && !search->used && lucky_num < 0){
      char *memAllocd = search->blockstart;
      
      if (search->size < n_bytes + sizeof(struct fl_node)){  /* if there's no room for the new overhead after allocating n_bytes */
	search->used = 1;                                    /* just allocate all the available memory */
	(handlers[handleCount].numNodes)--;    /* one free node used */
	return (void*) memAllocd;
      }
      
      node = search->blockstart + n_bytes;  /* start address of the next node */
      node->size = search->size - n_bytes - sizeof(struct fl_node);
      node->used = 0;
      node->blockstart = ((char*) node) + sizeof(struct fl_node); /* the start address of this page's allocatable memory */
      node->next = search->next;
      
      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      search->next = node;
      
      return (void*) memAllocd;
    }
    if (search->size == n_bytes && !search->used && lucky_num < 0){   /* if it fits exactly into the free memory, a special case */
      char *memAllocd = search->blockstart;
      
      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      (handlers[handleCount].numNodes)--;    /* one free node used */
      
      return (void*) memAllocd;
    }
    search = search->next;
  }
  return NULL;
}
