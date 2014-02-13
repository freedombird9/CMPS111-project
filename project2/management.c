/*management.c is a static library with three functions meminit()
 * memalloc() and memfree(), two linked lists store handlers for meminit */

#include <stdio.h>
#include <stdlib.h>

int max_handler;

/*store the handlers for free list and buddy allocator*/
struct fl_node{
    int handler_f;
    int type;
    void *p;
    struct fl_node* next;
};

struct bu_node{
    int handler_b;
    int min_pg;
    void *p;
    struct bu_node* next;
};

struct bu_node *head_bu = NULL;
struct fl_node *head_fl = NULL;


/*the meminit function initialize the memory allocator. It will initial n byte
 * space. The flags noted the type of allocator and parm1 is the minimum page size*/
int meminit (long n_bytes, unsigned int flags, int parm1, int *parm2){
    void *p;
    struct bu_node *newnode = malloc(sizeof(bu_node));
        max_handler = max_handler+1;
    /*call malloc and */
    p=malloc(n_bytes);
    if(flags==0X1){
        /*add the new handler to the linked list*/
        newnode->handler_b=max_handler;
        newnode->p=p;
        newnode->min_pg=parm1;
        newnode->next=NULL;
        if (List_bu==0){
            List_bu=newnode;
        }
        else{
            bu_node *walk = &List_bu;
            while(walk->next!=NULL)
                walk=walk->next;
            walk->next=newnode;
        }
    }
    if(flags==0x4||flags==0X0||flags==0X8||flags==0X10||flags==0X20||flags==0X40){

    }

   return max_handler;
}
