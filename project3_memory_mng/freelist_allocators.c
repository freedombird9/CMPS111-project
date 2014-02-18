#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "libmem.h"

/* #define _DEBUG_  */

/* first fit allocator */
void *ff_allot(struct handle *handlers, int handleNum, long n_bytes){
  struct fl_node *search = handlers[handleNum].freelist;

  while (search != NULL){
    struct fl_node *node;
    if (search->size > n_bytes && !search->used){
      char *memAllocd = search->blockstart;
      
      if (search->size <= n_bytes + sizeof(struct fl_node)){  /* if there's no room for the new overhead after allocating n_bytes */
	      search->used = 1;                                    /* just allocate all the available memory */
	      (handlers[handleNum].numNodes)--;    /* one free node used */
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
      (handlers[handleNum].numNodes)--;    /* one free node used */

      return (void*) memAllocd;
    }
    search = search->next;
  }
  printf("memalloc failed, not enough memory\n");
  return NULL;
}

/* next fit allocator */
void *nf_allot(struct handle *handlers, int handleNum, long n_bytes){    
  struct fl_node *search = handlers[handleNum].visited;
  if (search->next != NULL)  /* if the last visited node is not the last node */
    search = search->next;       /* start searching from the next node */
  else search = handlers[handleNum].freelist;  /* otherwise, start searching from the beginning  */
  while (search != NULL){
    struct fl_node *node;
    if (search->size > n_bytes && !search->used){
      char *memAllocd = search->blockstart;

      if (search->size <= n_bytes + sizeof(struct fl_node)){  /* if there's no room for the new overhead after allocating n_bytes */
	      search->used = 1;                                    /* just allocate all the available memory */
	      handlers[handleNum].visited = search;
	      (handlers[handleNum].numNodes)--;    /* one free node used */
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

      handlers[handleNum].visited = node;  /* keep the visited information */

      return (void*) memAllocd;
    }
    
    if (search->size == n_bytes && !search->used){   /* if it fits exactly into the free memory, a special case */
      char *memAllocd = search->blockstart;
      
      search->used = 1;          /* update old page's info */
      search->size = n_bytes;
      (handlers[handleNum].numNodes)--;    /* one free node used */

      handlers[handleNum].visited = node;
      
      return (void*) memAllocd;
   }
    search = search ->next;
  }
  printf("memalloc failed, not enough memory\n");
  return NULL;
}

/* best fit allocator */

void *bf_allot(struct handle *handlers, int handleNum, long n_bytes){
   struct fl_node *search = handlers[handleNum].freelist;
   struct fl_node *best;
   int findBest = 0;
 
   while (search != NULL){
     if (!search->used && search->size >= n_bytes){
       best = search;
       findBest = 1;
       break;
     }
     search = search->next;
   }
  if (!findBest){
    printf("memalloc failed, not enough memory\n");
    return NULL;
  }

   search = handlers[handleNum].freelist;
   
   while (search != NULL){
     if (search->size >= n_bytes && search->size < best->size && !search->used){ /* find the best fit node */
       best = search;
     }
     search = search->next;
   }
   
   struct fl_node *node;
   if (best->size > n_bytes && !best->used){
     char *memAllocd = best->blockstart;
     
     if (best->size <= n_bytes + sizeof(struct fl_node)){  /* if there's no room for the new overhead after allocating n_bytes */
       best->used = 1;                                    /* just allocate all the available memory */
       (handlers[handleNum].numNodes)--;    /* one free node used */
       return (void*) memAllocd;
     }
     
     node = best->blockstart + n_bytes;  /* start address of the next node */
     node->size = best->size - n_bytes - sizeof(struct fl_node);
     node->used = 0;
     node->blockstart = ((char*) node) + sizeof(struct fl_node); /* the start address of this page's allocatable memory */
     node->next = best->next;
     
     best->used = 1;          /* update old page's info */
     best->size = n_bytes;
     best->next = node;
     
     return (void*) memAllocd;
   }
   if (best->size == n_bytes && !best->used){   /* if it fits exactly into the free memory, a special case */
     char *memAllocd = best->blockstart;
     
     best->used = 1;          /* update old page's info */
     best->size = n_bytes;
     (handlers[handleNum].numNodes)--;    /* one free node used */
     
     return (void*) memAllocd;
   }
   printf("memalloc failed, not enough memory\n");
   return NULL;   
}


/* worst fit allocator */

void *wf_allot(struct handle *handlers, int handleNum, long n_bytes){
  struct fl_node *search = handlers[handleNum].freelist;
  struct fl_node *worst;
  int findWorst = 0;

  while (search != NULL){
    if (!search->used && search->size >= n_bytes){
      worst = search;
      findWorst = 1;
      break;
    }
    search = search->next;
  }
  if (!findWorst){
    printf("memalloc failed, not enough memory\n");
    return NULL;
  }
     
  search = handlers[handleNum].freelist;
  
  while (search != NULL){
    if (search->size >= n_bytes && search->size > worst->size && !search->used){ /* find the worst fit node */
      worst = search;
    }
    search = search->next;
  }
  
  struct fl_node *node;
  if (worst->size > n_bytes && !worst->used){
    char *memAllocd = worst->blockstart;
    
    if (worst->size <= n_bytes + sizeof(struct fl_node)){  /* if there's no room for the new overhead after allocating n_bytes */
      worst->used = 1;                                    /* just allocate all the available memory */
      (handlers[handleNum].numNodes)--;    /* one free node used */
      return (void*) memAllocd;
    }
    
    node = worst->blockstart + n_bytes;  /* start address of the next node */
    node->size = worst->size - n_bytes - sizeof(struct fl_node);
    node->used = 0;
    node->blockstart = ((char*) node) + sizeof(struct fl_node); /* the start address of this page's allocatable memory */
    node->next = worst->next;
    
    worst->used = 1;          /* update old page's info */
    worst->size = n_bytes;
    worst->next = node;
    
    return (void*) memAllocd;
  }
   if (worst->size == n_bytes && !worst->used){   /* if it fits exactly into the free memory, a special case */
     char *memAllocd = worst->blockstart;
     
     worst->used = 1;          /* update old page's info */
     worst->size = n_bytes;
     (handlers[handleNum].numNodes)--;    /* one free node used */
     
     return (void*) memAllocd;
   }
   printf("memalloc failed, not enough memory\n");
   return NULL;   
}

/* random fit allocator */

void *rf_allot(struct handle *handlers, int handleNum, long n_bytes){
  /* srand(time(NULL));       seed the random number generator  */ 
  struct fl_node *search = handlers[handleNum].freelist;
  int lucky_num = rand() % handlers[handleNum].numNodes;  /* use a lucky number to choose the nodes */

#ifdef _DEBUG_
  printf("lucky number: %d\n", lucky_num);
#endif
  
  while (search != NULL){
    struct fl_node *node;
    
    if (search->size >= n_bytes && !search->used){
      lucky_num--;
      
#ifdef _DEBUG_
      printf("lucky number: %d\n", lucky_num);
#endif
    }
    
    if (search->size > n_bytes && !search->used && lucky_num < 0){
      
#ifdef _DEBUG_
      printf("elligible node found\n");
#endif
      
      char *memAllocd = search->blockstart;
      
      if (search->size <= n_bytes + sizeof(struct fl_node)){  /* if there's no room for the overhead after allocating n_bytes */
	      search->used = 1;                                    /* just allocate all the available memory */
	      (handlers[handleNum].numNodes)--;    /* one free node used */
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
      (handlers[handleNum].numNodes)--;    /* one free node used */
      
      return (void*) memAllocd;
    }
    search = search->next;
  }
  printf("memalloc failed, not enough memory\n");
  return NULL;
}
