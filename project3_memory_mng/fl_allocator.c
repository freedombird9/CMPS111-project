#include <stdio.h>
#include <stdlib.h>
#include "libmem.h"

void *ff_allot(struct handle this_handle, int alot_bytes){
    struct fl_node *walk, *newnode, *temp;
    walk=this_handle->freelist;
    while(walk!=NULL){
        if((walk->used==0)&&((walk->size)>alot_bytes)){
            newnode=malloc(sizeof(struct fl_node));
            temp=walk->next;
            walk->used=1;
            walk->size=alot_bytes;
            walk->next=newnode;
            newnode->size=walk->size-alot_bytes;
            newnode->used=0;
            newnode->next=temp;
        }
        walk=walk->next;
    }
}
