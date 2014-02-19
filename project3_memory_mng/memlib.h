#ifndef _LIBMEM_H_
#define _LIBMEM_H_

#define MAX_HANDLE 10
int handleCount = 0;


/*store the handlers for free list and buddy allocator*/

struct fl_node{
    unsigned long size;
    void *memstart;
    struct fl_node* next;
};

struct bu_node{
    int handler_b;
    int min_pg;
    void *memstart;
    struct bu_node* next;
};


struct handle{
    unsigned int flags;
    void *memstart;
    long int n_bytes;

    struct fl_node *freelist;
    struct bu_node *bitmap;
} handlers[MAX_HANDLE];

struct bu_node *head_bu = NULL;
struct fl_node *head_fl = NULL;

#endif
