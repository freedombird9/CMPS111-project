#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libmem.h"

/*type 1 for change all its child to 1, 0 for change all children to 0 and 2 for only change one branch to 1*/
void modBitmap(int depth, struct bu_node *head,int index, int type);

void bu_free(struct bu_node *head,int index, unsigned long free_bytes);

int comp_pow(int num);

void *buddy_allot(struct handle *handlers, int handleCount, unsigned long alot_bytes){
    int min_pg=pow(2,handlers[handleCount].page_size);
    int level, current_level, length;
    int begin;
    int i;
    /*compute the level*/
    /*printf("%d ",handlers[handleCount].bu_depth);
    printf("%d ",alot_bytes);
    printf("%d \n",handlers[handleCount].page_size);*/
    if(alot_bytes%min_pg==0)
        if(alot_bytes==min_pg)
            level=handlers[handleCount].bu_depth;
        else
            level=handlers[handleCount].bu_depth-comp_pow(alot_bytes/min_pg)+1;
    else
        level=handlers[handleCount].bu_depth-(comp_pow(alot_bytes/min_pg));
    printf("level=%d\n",level);

    /*special case for alot_bytes=total length*/
    if(alot_bytes==handlers[handleCount].n_bytes){
        if(handlers[handleCount].bm_head[1].used==0){
            modBitmap(handlers[handleCount].bu_depth,handlers[handleCount].bm_head,0,1);
            return (void*) handlers[handleCount].memstart;
        }
        else
            printf("no much space to allocate\n");
    }

    /*general cases*/
    if(alot_bytes<handlers[handleCount].n_bytes){
        length=(int)(pow(2,level-1));
        begin=pow(2,level-2)+1;
        /*the */
        for (i=0;i<length;i=i+2){
            if((handlers[handleCount].bm_head[begin+i].used==0)&&(handlers[handleCount].bm_head[(begin+1)/2].used==1)){
                modBitmap(handlers[handleCount].bu_depth-level,handlers[handleCount].bm_head,begin+i,1);
                /*printf("find buddy %d\n",begin+1);
                printf("%d %d\n",handlers[handleCount].bm_head[begin+i].used,handlers[handleCount].bm_head[(begin+1)/2].used);
                printf("%p", handlers[handleCount].bm_head[begin+i].pointer);*/
                return (void*) handlers[handleCount].bm_head[begin+i].pointer;
            }
        }
        current_level=level-1;
        while(current_level>0){
            begin=pow(2,current_level-2);
            length=(int)(pow(2,current_level-1));
            printf("%d,%d,%d\n",current_level, begin,length);
            for (i=0;i<length;i=i+2){
                if((handlers[handleCount].bm_head[begin+i].used==0)&&(handlers[handleCount].bm_head[(begin+1)/2].used==1)){
                    modBitmap(handlers[handleCount].bu_depth-current_level, handlers[handleCount].bm_head,begin+i, 2);
                    return (void*) handlers[handleCount].bm_head[(begin+1)*(level-current_level)*2].pointer;
                }
            }
            current_level=current_level-1;
        }
        if(current_level==0){
            printf("on root\n");
        }
    }
    printf("don't have much space to allocate\n");
    return handlers[handleCount].memstart-1;
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

int comp_pow(int num){
    int pow=0;
    while(num>=1){
        num=num/2;
        pow++;
    }
    /*printf("pow=%d\n",pow);*/
    return pow;
}


/*
void bu_free(struct handle *handlers[handleCount], unsigned long free_bytes){
    int min_pg=handlers[handleCount]->page_size;
    int level, current_level, length;
    int begin;
    int i;

    if(free_bytes)


}
*/
