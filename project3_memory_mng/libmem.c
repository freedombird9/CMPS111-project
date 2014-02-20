#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "libmem.h"

 #define _VERBOSE_

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
    int length=(int)(power(depth)-1);
    struct bu_node *head = (struct bu_node*) malloc(length*sizeof(struct bu_node));
    int i , j=1,sum=0,level_count=0;
#ifdef _VERBOSE_
    printf("ini=%p end=%p\n",memstart,memstart+n_bytes);
#endif
    for(i=0;i<length;){
        if(i==0){
            head[i].pointer=(void*)memstart;
            head[i].used=0;
            head[i].a_used=0;
#ifdef _VERBOSE_
            printf("%d-%p-%d\n",i,head[i].pointer,head[i].used);
#endif
            i=i+1;
            continue;
        }
        /*the start of each line*/
        if(i==sum+power(j)){
            head[i].pointer=(void*)memstart+n_bytes*level_count/(int)power(j);
            head[i].used=0;
            head[i].a_used=0;
#ifdef _VERBOSE_
            printf("%d-%p-%d\n",i,head[i].pointer,head[i].used);
#endif
            j=j+1;
            sum=sum+power(j-1);
            i=i+1;
            level_count=0;
            continue;
        }
        else{
            head[i].pointer=(void*)memstart+n_bytes*level_count/(int)power(j);
            head[i].used=0;
            head[i].a_used=0;
#ifdef _VERBOSE_
            printf("%d-%p-%d ",i,head[i].pointer,head[i].used);
#endif
            i=i+1;
            level_count=level_count+1;
        }
    }
#ifdef _VERBOSE_
    printf("\n");
#endif

    return head;
}

void print_bu(struct bu_node *head,int depth){
    int length=(int)(power(depth)-1);
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
        if(i==sum+power(j)){
            /*printf("%p-",head[i].pointer);*/
            printf("%d \n",head[i].used); //;//printf("(s+p=%d i=%d)\n",sum+pow(2,j),i);
            j=j+1;
            sum=sum+power(j-1);
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

void status(int handle){
    printf("/*============== Status for buddy allocator ================*/\n");
    print_bu(handlers[handle].bm_head,handlers[handle].bu_depth);

}


void print_fl(struct handle *this_handle, int handle){
  struct fl_node *walk;
  walk = this_handle[handle].freelist;
  printf("handle number: %d\n",handle);
  while(walk != NULL){
    if(walk->used == 1)
      printf("used %d bytes + ",walk->size);     /*not sure about the format, so compare*/
    if(walk->used == 0)
      printf("unused %u bytes + ",walk->size);   /*and check*/
    walk = walk->next;
  }
  printf("\n");
}

void evaluate (int handle){

  struct fl_node *search = handlers[handle].freelist;
  struct fl_node *minHol = NULL;
  struct fl_node *maxHol = NULL;
  int holNum = 0;
  int holSize = 0;
  int initial = 0;
  float averSize = 0;
  float sqsum = 0;

  while (search != NULL){
    if (!search->used){        /* if it's a hole */
      if (initial == 0){
      	minHol = search;     /* initialize minHol and maxHol */
        maxHol = search;
      	initial = 1;
      }

      holNum++;
      holSize += search->size;
      if (search->size < minHol->size)
	      minHol = search;
      if (search->size > maxHol->size)
	      maxHol = search;
    }
    search = search->next;
  }

  if (holNum == 0){
    printf("/*============== Evaluations ================*/\n");
    printf("there's no hole\n");
    printf("number of holes: %d\n", 0);
    printf("min hole size: %d\n", 0);
    printf("max hole size: %d\n", 0);
    printf("average hole size: %d\n", 0);
    printf("std deviation of size: %d\n", 0);
    printf("/*============== Evaluations ================*/\n");

    return;
  }

  averSize = (float)holSize/(float)holNum;

  search = handlers[handle].freelist;

  while (search != NULL){
    if (!search->used){
      float diff = search->size - averSize;
      float squared = pow(diff, 2);
      sqsum += squared;
    }
    search = search->next;
  }

  float stddev = sqrt(sqsum/holNum);
  printf("/*============== Evaluations ================*/\n");
  printf("number of holes: %d\n", holNum);
  printf("min hole size: %d\n", minHol->size);
  printf("max hole size: %d\n", maxHol->size);
  printf("average hole size: %f\n", averSize);
  printf("std deviation of size: %f\n", stddev);
  printf("/*============== Evaluations ================*/\n");
}


int meminit(long n_bytes, unsigned int flags, int parm1){
  int i, j;
  int power = isValid(n_bytes);
  if (!power){
    printf("illegal memory size\n");
    return -1;
  }

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
#ifdef _VERBOSE_
    printf("after initiating memory:\n");
    print_fl(handlers,handleCount);
    printf("number of free nodes: %d\n", handlers[handleCount].numNodes);
#endif
  }

  else if (flags & 0x1){
    int depth = power - parm1 + 1;
    handlers[handleCount].flags=0x1;
    handlers[handleCount].bu_depth=depth;
    handlers[handleCount].page_size=parm1;
    handlers[handleCount].bm_head=initBitmap(depth, handlers[handleCount].memstart,n_bytes);

    print_bu(handlers[handleCount].bm_head,depth);
  }
  return (handleCount++);
}

void *memalloc(int handle, long n_bytes){
  void *p;
  if(handlers[handle].flags & 0x1){
    p = buddy_allot(handlers, handle, n_bytes);
#ifdef _VERBOSE_
    print_bu(handlers[handle].bm_head, handlers[handle].bu_depth);
#endif
  }

  else if(handlers[handle].flags & 0x4){
    /*determine type*/
    if(handlers[handle].flags == (0x0 | 0x4)){
      /*call first fit*/
#ifdef _VERBOSE_
      printf("first fit called\n");
#endif
      p = ff_allot(handlers, handle, n_bytes);
    }
    if(handlers[handle].flags == (0x8 | 0x4)){
      /*call next fit*/
#ifdef _VERBOSE_
      printf("next fit called\n");
#endif
      p = nf_allot(handlers, handle, n_bytes);
    }
    if(handlers[handle].flags == (0x10 | 0x4)){
      /*call best fit*/
#ifdef _VERBOSE_
      printf("best fit called\n");
#endif
      p = bf_allot(handlers, handle, n_bytes);
    }
    if(handlers[handle].flags == (0x20 | 0x4)){
      /*call worst fit*/
#ifdef _VERBOSE_
      printf("worst fit called\n");
#endif
      p = wf_allot(handlers, handle, n_bytes);
    }
    if(handlers[handle].flags == (0x40 | 0x4)){
      /*call random fit*/
#ifdef _VERBOSE_
      printf("random fit called\n");
#endif
      p = rf_allot(handlers, handle, n_bytes);
    }
#ifdef _VERBOSE_
    printf("after allocation:\n");
    print_fl(handlers, handle);
    printf("number of free nodes: %d\n", handlers[handle].numNodes);
#endif
  }
  return p;
}

void memfree (void *region){
  int i;
  int bu_return;
  for (i = 0; i != handleCount; i++){   /* search for the right handler */
    if(handlers[i].flags & 0x1){
      bu_return=bu_free(handlers[i],region);
      //printf("return =%d\n",bu_return);
      if(bu_return==1){
#ifdef _VERBOSE_
	print_bu(handlers[i].bm_head,handlers[i].bu_depth);
#endif
	break;
      }
    }/*end case for buddy allocator*/
    if (handlers[i].flags & 0x4) {
      if ((char*) region >= handlers[i].memstart && (char*) region <= handlers[i].memstart + handlers[i].n_bytes){    /* if find the right handler */
	struct fl_node *search = handlers[i].freelist;
	struct fl_node *after = search->next;
	struct fl_node *pre = search;
	while (search != NULL){
	  if ((char*) region == search->blockstart){
	    if (search == handlers[i].freelist){     /* if the node to be freed is the first node */
	      if ( after != NULL && after->used == 0){     /* if the next node is also free, merge the two */
		search->used = 0;
		search->size = search->size + sizeof(struct fl_node) + after->size;
		search->next = after->next;
		after->used = 0; after->blockstart = 0; after->size = 0; after->next = 0;   /* clean the node freed */
#ifdef _VERBOSE_
		printf("after freeing the first node:\n");
		print_fl(handlers,i);
		printf("number of free nodes: %d\n", handlers[i].numNodes);
#endif
		return;
	      }
	      else {
		search->used = 0;
		(handlers[i].numNodes)++;
		return;
	      }
	    }
	    else if (search->next == NULL){     /* if the node to be freed is the last node */
	      if (pre->used == 0){     /* if its previous node is also free, merge them */
		pre->size = pre->size + sizeof(struct fl_node) + search->size;
		pre->next = NULL;
		search->used = 0; search->blockstart = 0; search->size = 0; search->next = 0;   /* clean the node freed */
#ifdef _VERBOSE_
		printf("after freeing the last node:\n");
		print_fl(handlers,i);
		printf("number of free nodes: %d\n", handlers[i].numNodes);
#endif
		return;
	      }
	      else {
	        search->used = 0;
		(handlers[i].numNodes)++;
#ifdef _VERBOSE_
		printf("after freeing the last node:\n");
		print_fl(handlers,i);
		printf("number of free nodes: %d\n", handlers[i].numNodes);
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
		  (handlers[i].numNodes)--;
#ifdef _VERBOSE_
		  printf("after freeing the middle node whose both neighbors are free:\n");
		  print_fl(handlers,i);
		  printf("number of free nodes: %d\n", handlers[i].numNodes);
#endif
		  return;
		}
		else{       /* if only the previous node is free */
		  pre->size = pre->size + sizeof(struct fl_node) + search->size;
		  pre->next = search->next;
		  search->used = 0; search->blockstart = 0; search->size = 0; search->next = 0;   /* clean the node freed */
#ifdef _VERBOSE_
		  printf("after freeing the middle node whose previous neighbor is free:\n");
		  print_fl(handlers,i);
		  printf("number of free nodes: %d\n", handlers[i].numNodes);
#endif
		  return;
		}
	      }
	      else if (after->used == 0){     /* if the next node is free */
		search->size = search->size + sizeof(struct fl_node) + after->size;
		search->next = after->next;
		search->used = 0;
		after->used = 0; after->blockstart = 0; after->size = 0; after->next = 0;   /* clean the node freed */
#ifdef _VERBOSE_
		printf("after freeing the middle node whose next neighbor is free:\n");
		print_fl(handlers,i);
		printf("number of free nodes: %d\n", handlers[i].numNodes);
#endif
		return;
	      }
	      else{
		search->used = 0;
		(handlers[i].numNodes)++;
#ifdef _VERBOSE_
		printf("after freeing the middle node with no free neighbors (no merge):\n");
		print_fl(handlers,i);
		printf("number of free nodes: %d\n", handlers[i].numNodes);
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
    }/*end case free list*/
  }/*end for*/
}/*end function*/
