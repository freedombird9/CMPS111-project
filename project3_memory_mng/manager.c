#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

void initBitmap(int depth, struct bu_node *head, char *memstart, long int n_bytes){
    int length=(int)(pow(2,depth)-1);
    struct bu_node bitmap[length];
    head=bitmap;
    int i , j=0,level_count=0;
    for(i=0;i<length;i++){
        if(i==pow(2,j)){
            head[i].pointer=memstart;
            continue;
        }
        else{
            bitmap[i].pointer=memstart+n_bytes*((int)(i-pow(2,j-1))/(int)pow(2,j));
            level_count=level_count+1;
            if(level_count==(int)(pow(2,j)-pow(2,j-1)))
                    j=j+1;
        }
    }

    /*
    root->used = 0;
  if(depth == 0){
    root->left = NULL;
    root->right = NULL;
  }
  while(depth > 0){
    depth=depth-1;
    root->left = malloc(sizeof(struct bu_node));
    root->right = malloc(sizeof(struct bu_node));
    root->right->parent=root;
    root->left->parent=root;
    root->left->pointer=memstart;
    root->right->pointer=memstart+n_bytes/2;
    initBitmap(depth, root->left, memstart, n_bytes/2);
    initBitmap(depth, root->right,memstart+n_bytes/2,n_bytes/2);
  }
  */
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
    handlers[handleCount].freelist = handlers[handleCount].memstart;   /* keep the data structure inside the memory region */
    handlers[handleCount].freelist->blockstart = handlers[handleCount].memstart + sizeof(struct fl_node);
    handlers[handleCount].freelist->size = n_bytes - sizeof(struct fl_node);
    handlers[handleCount].freelist->used = 0;		/* not used, allocatable */
    handlers[handleCount].freelist->next = NULL;
    handlers[handleCount].visited = handlers[handleCount].freelist;
    handlers[handleCount].numNodes = 1;     /* we got one node with free memory after initiation */
  }

  else if (flags & 0x1){
    int depth = power - parm1 + 1;
    handlers[handleCount].memstart = malloc(n_bytes);
    handlers[handleCount].bm_head = malloc((pow(2,depth)-1)*sizeof(struct bu_node));
    initBitmap(depth, handlers[handleCount].bm_head,handlers[handleCount].memstart,n_bytes);
    handlers[handleCount].bu_depth=depth;
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
        p=ff_allot(handlers, handle, n_bytes);
      }
      if(handlers[handle].flags == (0x8 | 0x4)){
      /*call next fit*/
        p=nf_allot(handlers, handle, n_bytes);
      }
      if(handlers[handle].flags == (0x10 | 0x4)){
      /*call best fit*/
        p=bf_allot(handlers, handle, n_bytes);
      }
      if(handlers[handle].flags == (0x20 | 0x4)){
        /*call worst fit*/
        p=wf_allot(handlers, handle, n_bytes);
      }
      if(handlers[handle].flags == (0x40 | 0x4)){
        /*call random fit*/
        p=rf_allot(handlers, handle, n_bytes);
          }
      }
      return p;
}

void memfree (void *region){
  int i;
  for (i = 0; i != handleCount; i++){   /* search for the right handler */
    if (handlers[i].flags & 0x4) {
      if ((char*) region >= handlers[i].memstart && (char*) region <= handlers[i].memstart + handlers[i].n_bytes){    /* if find the right handler */
	struct fl_node *search = handlers[i].freelist;
	struct fl_node *after = search->next;
	struct fl_node *pre = search;
	while (search != NULL){
	  if ((char*) region == search->blockstart){
	    handlers[i].numNodes++;
	    if (search == handlers[i].freelist){     /* if the node to be freed is the first node */
	      if ( after->used == 0){     /* if the next node is also free, merge the two */
		search->used = 0;
		search->size = search->size + sizeof(struct fl_node) + after->size;
		search->next = after->next;
		after->used = 0; after->blockstart = 0; after->size = 0; after->next = 0;   /* clean the node freed */
	      }
	      else search->used = 0;
	    }
	    if (search->next == NULL){     /* if the node to be freed is the last node */
	      if (pre->used == 0){     /* if its previous node is also free, merge them */
		pre->size = pre->size + sizeof(struct fl_node) + search->size;
		pre->next = NULL;
		search->used = 0; search->blockstart = 0; search->size = 0; search->next = 0;   /* clean the node freed */
	      }
	      else search->used = 0;
	    }
	    else{         /* if the node to be freed is in between */
	      if (pre->used == 0){     /* if the previous one is also free, merge them */
		if (after->used == 0){      /* if the nodes before and after are both free */
		  pre->size = pre->size + 2*sizeof(struct fl_node) + search->size + after->size;
		  pre->next = after->next;
		  after->used = 0; after->blockstart = 0; after->size = 0; after->next = 0;   /* clean the node freed */
		  search->used = 0; search->blockstart = 0; search->size = 0; search->next = 0;   /* clean the node freed */
		}
		else{       /* if only the previous node is free */
		  pre->size = pre->size + sizeof(struct fl_node) + search->size;
		  pre->next = search->next;
		  search->used = 0; search->blockstart = 0; search->size = 0; search->next = 0;   /* clean the node freed */
		}
	      }
	      if (after->used == 0 && pre->used != 0){     /* if the next node is also free */
		search->size = search->size + sizeof(struct fl_node) + after->size;
		search->next = after->next;
		after->used = 0; after->blockstart = 0; after->size = 0; after->next = 0;   /* clean the node freed */
	      }
	    }
	    else search->used = 0;  /* only the current node is free */
	  }
	  pre = search;
	  search = search->next;
	}   /* end while */
      }   /* end handler search */
    }   /* end case for freelist */
  }
}

void print_fl(struct handle *this_handle, int handle){
    struct fl_node *walk;
    walk=this_handle->freelist;
    printf("The allocation in handle %d\n",handle);
    while(walk!=NULL){
        if(walk->used==1)
            printf("used %ld bytes + ",walk->size);     /*not sure about the format, so compare*/
        if(walk->used==0)
            printf("unused %lu bytes + ",walk->size);   /*and check*/
    }
    printf("\n");
}

void print_bu(struct bu_node *head,int depth){
    int length=(int)(pow(2,depth)-1);
    struct bu_node bitmap[length];
    int i , j=0,level_count=0;
    for(i=0;i<length;i++){
        if(i==pow(2,j)){
            printf("%d \n",head[i].used);
            continue;
        }
        else{
            printf("%d ",head[i].used);
            level_count=level_count+1;
            if(level_count==(int)(pow(2,j)-pow(2,j-1)))
            j=j+1;
        }
    }
}
