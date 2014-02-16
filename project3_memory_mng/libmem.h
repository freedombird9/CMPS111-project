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
    char *pointer;
};

struct handle{
    unsigned int flags;
    void *memstart;
    long int n_bytes;
    int numNodes;      /* track the number of nodes that have unallocated memory */

    struct fl_node *freelist;
    struct fl_node *visited    /* store the previously visited node ,used for next fit */
    struct bu_node *bm_head;
    int bu_depth;
    int page_size;
} handlers[MAX_HANDLE];

#endif

