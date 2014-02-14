#ifndef _LIBMEM_H_
#define _LIBMEM_H_

#define MAX_HANDLE 20
int handleCount = 0;


/*store the handlers for free list and buddy allocator*/

struct fl_node{
    unsigned long size;
    int used;
    void *blockstart;
    struct fl_node *next;
};

struct bu_node{
    int used;
    struct bu_node *parent;
    struct bu_node *left;
    struct bu_node *right;
};

struct handle{
    unsigned int flags;
    void *memstart;
    long int n_bytes;

    struct fl_node *freelist;
    struct bu_node *bitmap;
    int bu_depth;
    int page_size;
} handlers[MAX_HANDLE];

#endif

