#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libmem.h"

/*type 1 for change all its child to 1, 0 for change all children to 0 and 2 for only change one branch to 1*/
void modBitmap(int depth, struct bu_node *head,int index, int type);


void *buddy_allot(struct handle *this_handle, unsigned long alot_bytes){
    int min_pg=this_handle->page_size;
    int level, current_level, length;
    int begin;
    int i;
    if(alot_bytes%min_pg==0)
        level=this_handle->bu_depth-(alot_bytes/this_handle->page_size)/2;
    else
        level=this_handle->bu_depth-(alot_bytes/this_handle->page_size+1)/2;

    if(alot_bytes==this_handle->n_bytes){
        if(this_handle->bm_head[1]==0){
            modBitmap(0,this_handle->bm_head,1);
            return this_handle->memstart;
        }
        else
            printf("no much space to allocate\n");
    }

    if(alot_bytes<this_handle->n_bytes){
        length=(int)pow(2,level)-pow(2,level-1);
        begin=pow(2,level);
        for (i=0;i<length;i=i+2){
            if((this_handle->bm_head[begin+i]==0)(this_handle->bm_head[(begin+1)/2]==1))
                /*this_handle->bm_head[begin+i]=1;*/
                modBitmap(this_handle->bu_depth-level,this_handle->bm_head,begin+i,1);
                return this_handle->bm_head[begin+i]->pointer;
            }
        current_level=level-1;
        while(current_level>0){
            begin=pow(2,current_level);
            length=(int)pow(2,current_level)-pow(2,current_level-1);
            for (i=0;i<length;i=i+2){
                if((this_handle->bm_head[begin+i]==0)(this_handle->bm_head[(begin+1)/2]==1)){
                    /*this_handle->bm_head[begin+i]=1;*/
                    modBitmap(this_handle->bu_depth-current_level, this_handle->bm_head,begin+i, 2);
                   /* this_handle->bm_head[(begin+i)*2]==1;
                    this_handle->bm_head[(begin+i)*2+1]==0;*/
                    return this_handle->bm_head[(begin+1)*(level-current_level)*2]->pointer;
                }
            }
        }

    }
}

void modBitmap(int depth, struct bu_node *head, int index, int type){
    if(type==1){
        head[index].used=1;
        if(depth>0){
            modBitmap(depth-1, head, index*2, 1);
            modBitmap(depth-1, head, index*2+1, 1);
        }
    }
    if(type==0){
        head[index].used=0;
        if(depth>0){
            modBitmap(depth-1, head, index*2, 0);
            modBitmap(depth-1, head, index*2+1, 0);
        }
    }
    if(type==2){
        head[index].used=1;
        if(depth>0){
            modBitmap(depth-1, head, index*2, 1);
            modBitmap(depth-1, head, index*2+1, 0);
        }
    }
}

