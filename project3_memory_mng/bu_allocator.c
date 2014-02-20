#include <stdio.h>
#include <stdlib.h>
#include "libmem.h"


void *buddy_allot(struct handle *handlers, int handlecnt, unsigned long alot_bytes){
    int min_pg=power(handlers[handlecnt].page_size);
    int level, current_level, length;
    int begin;
    int flag=0;     /*if an block is found*/
    int i;
    /*compute the level*/
    if(alot_bytes%min_pg==0){
        if(alot_bytes==min_pg)
            level=handlers[handlecnt].bu_depth;
        else
            level=handlers[handlecnt].bu_depth-comp_pow(alot_bytes/min_pg)+1;
    }
    else
        level=handlers[handlecnt].bu_depth-(comp_pow(alot_bytes/min_pg));

    /*special case for alot_bytes=total length*/
    if(alot_bytes==handlers[handlecnt].n_bytes){
        if(handlers[handlecnt].bm_head[1].used==0){
            modBitmap(handlers[handlecnt].bu_depth,handlers[handlecnt].bm_head,0,1);
            handlers[handlecnt].bm_head[1].a_used=1;
            flag=1;
            return (void*) handlers[handlecnt].memstart;
        }
        else
            printf("no much space to allocate\n");
    }

    /*general cases*/
    if(alot_bytes<handlers[handlecnt].n_bytes){
        length=(int)(power(level-1));
        begin=power(level-1)-1;
        for (i=0;i<length;i=i+1){
            if((handlers[handlecnt].bm_head[begin+i].used==0)&&(handlers[handlecnt].bm_head[find_parents(begin+i)].used==1)){
                printf(" %d %d %d\n",level,begin,i);
                modBitmap(handlers[handlecnt].bu_depth-level,handlers[handlecnt].bm_head,begin+i,1);
                handlers[handlecnt].bm_head[begin+i].a_used=1;
                flag=1;
                return (void*) handlers[handlecnt].bm_head[begin+i].pointer;
            }
        }
        current_level=level-1;
        while(current_level>0){
            begin=power(current_level-1)-1;
            length=(int)(power(current_level-1));
            for (i=0;i<length;i=i+1){
                if((handlers[handlecnt].bm_head[begin+i].used==0)&&(handlers[handlecnt].bm_head[find_parents(begin+i)].used==1)){
                    printf("%d,%d,%d\n",current_level, begin,i);
                    modBitmap(handlers[handlecnt].bu_depth-current_level, handlers[handlecnt].bm_head,begin+i, 2);
                    modBitmap(handlers[handlecnt].bu_depth-current_level,handlers[handlecnt].bm_head,find_gradchi((begin+i),level-current_level),1);
                    handlers[handlecnt].bm_head[find_gradchi((power(current_level-1)-2+begin+i),level-current_level)].a_used=1;
                    flag=1;
                    return (void*) handlers[handlecnt].bm_head[power(current_level-1)-2+begin+i].pointer;
                }
            }
            current_level=current_level-1;
        }
        if((current_level==0)&&handlers[handlecnt].bm_head[0].used==0){
            modBitmap(handlers[handlecnt].bu_depth, handlers[handlecnt].bm_head, 0, 2);
            modBitmap(handlers[handlecnt].bu_depth-level, handlers[handlecnt].bm_head, (int)power(level-1)-1, 1);
            handlers[handlecnt].bm_head[power(level-1)-1].a_used=1;
            return (void*) handlers[handlecnt].memstart;
        }
    }
    printf("don't have much space to allocate\n");
    return handlers[handlecnt].memstart-1;
}

void modBitmap(int depth, struct bu_node *head, int index, int type){
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
    /*printf("in buddy free find%p\n",free_bytes);*/
    for(i=length-1;i>0;i--){
        /*printf("i=%d p=%p\n",i,this_handle.bm_head[i].pointer);*/
        if ((this_handle.bm_head[i].pointer==free_bytes)&&(this_handle.bm_head[i].a_used==1)){
            this_handle.bm_head[i].used=0;
            this_handle.bm_head[i].a_used=0;
            flag=1;
            break;
        }
    }
    if(flag==1){
        modBitmap(this_handle.bu_depth-comp_pow(i)+1, this_handle.bm_head, i, 0);
        if(find_buddy(i)!=-1){
            //printf("%d,%d, %d,%d\n",flag,i,find_buddy(i),this_handle.bm_head[find_buddy(i)].used);
            while(this_handle.bm_head[find_buddy(i)].used==0){
                modBitmap(this_handle.bu_depth-comp_pow(find_parents(i))+1, this_handle.bm_head, find_parents(i), 0);
                i=find_parents(i);
                /*printf("%d",i);*/
                if(i==-1){
                    break;
                }
            }
        }
    }
    /*printf("flag=%d\n",flag);*/
    return flag;
}


