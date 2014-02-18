#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libmem.h"

int handleCount = 0;
int isValid(long number){
  int power=0;
  while(number % 2 == 0){
    number /= 2;
    power++;
    if (number == 1){
      /*printf("power=%d number=%ld\n",power,number);*/
      return power;
    }
  }
  return 0;
}

struct bu_node * initBitmap(int depth, char *memstart, long int n_bytes){
    int length=(int)(pow(2,depth)-1);
    struct bu_node *head = (struct bu_node*) malloc(length*sizeof(struct bu_node));
    int i , j=1,sum=0,level_count=0;
    printf("ini=%p end=%p\n",memstart,memstart+n_bytes);
    for(i=0;i<length;){
        if(i==0){
            head[i].pointer=memstart;
            head[i].used=0;
            printf("%d-%p-%d\n",i,head[i].pointer,head[i].used);
            i=i+1;
            continue;
        }
        /*the start of each line*/
        if(i==sum+pow(2,j)){
            head[i].pointer=memstart+n_bytes*level_count/(int)pow(2,j);
            head[i].used=0;
            printf("%d-%p-%d\n",i,head[i].pointer,head[i].used);
            j=j+1;
            sum=sum+pow(2,j-1);
            i=i+1;
            level_count=0;
            continue;
        }
        else{
            head[i].pointer=memstart+n_bytes*level_count/(int)pow(2,j);
            head[i].used=0;
            printf("%d-%p-%d ",i,head[i].pointer,head[i].used);
            i=i+1;
            level_count=level_count+1;
        }
    }
    printf("\n");
    return head;
}

void print_bu(struct bu_node *head,int depth){
    int length=(int)(pow(2,depth)-1);
    int i , j=1,sum=0;
    /*printf("in print_bu depth=%d\n",depth);*/
    /*printf("%p-\n",&head[0]);*/
    /*printf("%d \n",head_flld[0].used);*/
    for(i=0;i<length;){
        if(i==0){
            /*printf("%p-",head[i].pointer);*/
            printf("%d \n",head[i].used);
            i=i+1;
            continue;
        }
        if(i==sum+pow(2,j)){
            /*printf("%p-",head[i].pointer);*/
            printf("%d \n",head[i].used); //;//printf("(s+p=%d i=%d)\n",sum+pow(2,j),i);
            j=j+1;
            sum=sum+pow(2,j-1);
            i=i+1;
            continue;
        }
        else{
            /*printf("%p-",head[i].pointer);*/
            printf("%d ",head[i].used);
            i=i+1;
        }
    }
}

void print_fl(struct handle *this_handle, int handle){
    struct fl_node *walk;
    walk=this_handle[handle].freelist;
    printf("The allocation in handle %d\n",handle);
    while(walk!=NULL){
        if(walk->used==1)
            printf("used %ld bytes + ",walk->size);     /*not sure about the format, so compare*/
        if(walk->used==0)
            printf("unused %lu bytes + ",walk->size);   /*and check*/
        walk = walk->next;
    }
    printf("\n");
}


int meminit(long n_bytes, unsigned int flags, int parm1){
  int i, j;
  int power = isValid(n_bytes);
  if (!power)
    return -1;
  printf("power=%d parm1=%d\n",power,parm1);
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
    printf("depth=%d\n",depth);
    handlers[handleCount].flags=0x1;
    handlers[handleCount].memstart = malloc(n_bytes);
    handlers[handleCount].bm_head = malloc((pow(2,depth)-1)*sizeof(struct bu_node));
    handlers[handleCount].bu_depth=depth;
    handlers[handleCount].page_size=parm1;
    handlers[handleCount].bm_head=initBitmap(depth, handlers[handleCount].memstart,n_bytes);
    /*printf("%p %p %d\n",&handlers[handleCount].bm_head[0].pointer,&handlers[handleCount].bm_head[0].used,depth);
    printf("head=%p\n",&handlers[handleCount].bm_head[0]);*/
    print_bu(handlers[handleCount].bm_head,depth);
    /*printf("memstart=%p\n", handlers[handleCount].memstart);*/
  }
  return handleCount++;
}

void *memalloc(int handle, long n_bytes){
      void *p;
      if(handlers[handle].flags & 0x1){
        /*call buddy allocator*/
        p=buddy_allot(handlers, handle, n_bytes);
        print_bu(handlers[handle].bm_head,handlers[handle].bu_depth);
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
  printf("in free\n");
  printf("lslsls\n");
  printf("---%d",handleCount);
  for (i = 0; i != handleCount; i++){   /* search for the right handler */
    printf("i=%d",i);
      if(handlers[i].flags & 0x1){
        /*buddy allocator*/

        if(bu_free(handlers[i],region))
            break;

    }
    if (handlers[i].flags & 0x4) {
      if ((char*) region >= handlers[i].memstart && (char*) region <= handlers[i].memstart + handlers[i].n_bytes){    /* if find the right handler */
	struct fl_node *search = handlers[i].freelist;
	struct fl_node *after = search->next;
	struct fl_node *pre = search;
	while (search != NULL){
	  if ((char*) region == search->blockstart){
	    (handlers[i].numNodes)++;
	    if (search == handlers[i].freelist){     /* if the node to be freed is the first node */
	      if ( after != NULL && after->used == 0){     /* if the next node is also free, merge the two */
		search->used = 0;
		search->size = search->size + sizeof(struct fl_node) + after->size;
		search->next = after->next;
		after->used = 0; after->blockstart = 0; after->size = 0; after->next = 0;   /* clean the node freed */
#ifdef _DEBUG_
		printf("after freeing the first node:\n");
		print_fl(handlers,i);
#endif
		return;
	      }
	      else {search->used = 0; return; }
	    }
	    else if (search->next == NULL){     /* if the node to be freed is the last node */
	      if (pre->used == 0){     /* if its previous node is also free, merge them */
		pre->size = pre->size + sizeof(struct fl_node) + search->size;
		pre->next = NULL;
		search->used = 0; search->blockstart = 0; search->size = 0; search->next = 0;   /* clean the node freed */
#ifdef _DEBUG_
		printf("after freeing the last node:\n");
		print_fl(handlers,i);
#endif
		return;
	      }
	      else {
	        search->used = 0;
#ifdef _DEBUG_
		printf("after freeing the last node:\n");
		print_fl(handlers,i);
#endif
	        return;
	      }
	    }
	    else{         /* if the node to be freed is in between */
	      if (pre->used == 0){     /* if the previous one is also free, merge them */
		if (after->used == 0){      /* if the nodes before and after are both free */
		  pre->size = pre->size + 2*sizeof(struct fl_node) + search->size + after->size;
		  pre->next = after->next;
		  after->used = 0; after->blockstart = 0; after->size = 0; after->next = 0;   /* clean the node freed */
		  search->used = 0; search->blockstart = 0; search->size = 0; search->next = 0;   /* clean the node freed */
#ifdef _DEBUG_
		  printf("after freeing the middle node:\n");
		  print_fl(handlers,i);
#endif
		  return;
		}
		else{       /* if only the previous node is free */
		  pre->size = pre->size + sizeof(struct fl_node) + search->size;
		  pre->next = search->next;
		  search->used = 0; search->blockstart = 0; search->size = 0; search->next = 0;   /* clean the node freed */
#ifdef _DEBUG_
		  printf("after freeing the middle node:\n");
		  print_fl(handlers,i);
#endif
		  return;
		}
	      }
	      else if (after->used == 0){     /* if the next node is also free */
		search->size = search->size + sizeof(struct fl_node) + after->size;
		search->next = after->next;
		after->used = 0; after->blockstart = 0; after->size = 0; after->next = 0;   /* clean the node freed */
#ifdef _DEBUG_
		printf("after freeing the middle node:\n");
		print_fl(handlers,i);
#endif
		return;
	      }
	      else {
		search->used = 0;
#ifdef _DEBUG_
		printf("after freeing the middle node:\n");
		print_fl(handlers,i);
#endif
		return;
	      }  /* only the current node is free */
	    }
	  }
	  pre = search;
	  search = after;
	  after = after->next;
	}   /* end while */
      }   /* end handler search */
    }   /* end case for freelist */
  }
}


