#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libmem.h"


int power(int num);
int find_buddy(int num);
int find_parents(int num);
int comp_pow(int num);


void modBitmap(int depth, struct bu_node *head, int index, int type);


void *buddy_allot(struct handle *handlers, int handleCount, unsigned long alot_bytes){
    int min_pg=power(handlers[handleCount].page_size);
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
            /*printf("%p %d\n",handlers[handleCount].bm_head[1].pointer,handlers[handleCount].bm_head[1].used);*/
            modBitmap(handlers[handleCount].bu_depth,handlers[handleCount].bm_head,0,1);
            return (void*) handlers[handleCount].memstart;
        }
        else
            printf("no much space to allocate\n");
    }

    /*general cases*/
    if(alot_bytes<handlers[handleCount].n_bytes){
        length=(int)(power(level-1));
        begin=power(level-2)+1;
        /*the */
        for (i=0;i<length;i=i+1){
            if((handlers[handleCount].bm_head[begin+i].used==0)&&(handlers[handleCount].bm_head[find_parents(begin+i)].used==1)){
                printf("found %d\n",begin+i);
                modBitmap(handlers[handleCount].bu_depth-level,handlers[handleCount].bm_head,begin+i,1);
                /*printf("find buddy %d\n",begin+1);
                printf("%d %d\n",handlers[handleCount].bm_head[begin+i].used,handlers[handleCount].bm_head[(begin+1)/2].used);
                printf("%p", handlers[handleCount].bm_head[begin+i].pointer);*/
                return (void*) handlers[handleCount].bm_head[begin+i].pointer;
            }
        }
        current_level=level-1;
        while(current_level>0){
            begin=power(current_level-2);
            length=(int)(pow(2,current_level-1));
            printf("%d,%d,%d\n",current_level, begin,length);
            for (i=0;i<length;i=i+1){
                if((handlers[handleCount].bm_head[(int)power(current_level-1)-2+begin+i].used==0)&&(handlers[handleCount].bm_head[find_parents(power(current_level-1)-2+begin+i)].used==1)){
                    printf("at %d found %d\n",current_level,begin+i);
                    modBitmap(handlers[handleCount].bu_depth-current_level, handlers[handleCount].bm_head,power(current_level-1)-2+begin+i, 2);
                    modBitmap(handlers[handleCount].bu_depth-current_level,handlers[handleCount].bm_head,find_gradchi((power(current_level-1)-2+begin+i),level-current_level),1);
                    /*printf("%d\n",find_gradchi((power(current_level-1)-2+begin+i),level-current_level));*/
                    return (void*) handlers[handleCount].bm_head[(begin+1)*(level-current_level)*2].pointer;
                }
            }
            current_level=current_level-1;
        }
        if(current_level==0){
            printf("on root\n");
            /*printf("%p\n",handlers[handleCount].bm_head[2].pointer);*/
            modBitmap(handlers[handleCount].bu_depth, handlers[handleCount].bm_head, 0, 2);
            modBitmap(handlers[handleCount].bu_depth-level, handlers[handleCount].bm_head, (int)power(level-1)-1, 1);
            /*printf("%d ",(int)pow(2,level-1)-1);*/
            /*printf("return %p \n", handlers[handleCount].bm_head[(int)pow(2,level-2)+1].pointer);*/
            return (void*) handlers[handleCount].bm_head[(int)power(level-2)+1].pointer;
        }
    }
    printf("don't have much space to allocate\n");
    return handlers[handleCount].memstart-1;
}

void modBitmap(int depth, struct bu_node *head, int index, int type){
    /*printf("in mod index=%d depth=%d\n",index,depth);*/
    /*printf("%p,%d\n",head[index].pointer,head[index].used);*/
    /*if(index==0){
        if(type==1||type==2)
            head[index].used=1;
        else
            head[index].used=0;
        index=index+1;
        depth=depth-1;
    }
    if(index>0){*/
        if(type==1){
            head[index].used=1;
            if(depth>=1){
                modBitmap(depth-1, head, index*2+1, 1);
                modBitmap(depth-1, head, index*2+2, 1);
            }
        }
        if(type==0){
            head[index].used=0;
            if(depth>=1){
                modBitmap(depth-1, head, index*2+1, 0);
                modBitmap(depth-1, head, index*2+2, 0);
            }
        }
        if(type==2){
            head[index].used=1;
            if(depth>1){
                modBitmap(depth-1, head, index*2+1, 2);
                modBitmap(depth-1, head, index*2+2, 0);
            }
        }
   // }
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

int find_parents(int num){
    if(num%2==0)
        return num/2-1;
    else
        return num/2;
}

int find_gradchi(int num, int level){
    /*printf("in find_gradchid level=%d num=%d\n",level,num);*/
    if(level>0){
        num=num*2+1;
        return find_gradchi(num,level-1);
    }
    else
        return num;

}

int find_buddy(int num){
    if (num==0)
        return -1;
    if(num%2==0)
        return num-1;
    else
        return num+1;

}

int power(int num){
    int i,result=1;
    for (i=0;i<num;i++){
        result=result*2;
    }
    return result;


}

int bu_free(struct handle this_handle, void *free_bytes){
    int length=(int)(power(this_handle.bu_depth)-1);
    int i, flag=0;
    for(i=0;i<length;i++){
        if (this_handle.bm_head[i].pointer==free_bytes){
            this_handle.bm_head[i].used=0;
            flag=1;
            break;
        }
    }
    modBitmap(this_handle.bu_depth-comp_pow(i)+1, this_handle.bm_head, i, 0);
    while(find_buddy(i)!=-1){
        while(this_handle.bm_head[find_buddy(i)].used==0){
            modBitmap(this_handle.bu_depth-comp_pow(find_parents(i))+1, this_handle.bm_head, find_parents(i), 0);
        }
    }

    return flag;
}


