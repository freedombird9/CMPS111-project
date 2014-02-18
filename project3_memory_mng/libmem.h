#ifndef LIBMEM_H
#define LIBMEM_H

#define MAX_HANDLE 64

/*store the handlers for free list and buddy allocator*/

struct fl_node{
    int size;
    int used;
    char *blockstart;
    struct fl_node* next;
};

struct bu_node{
    int used;
    char *pointer;
};

struct handle{
    unsigned int flags;
    char *memstart;      /* start point of the memory region to be managed */
    long int n_bytes;    /* the size of that region */
    int numNodes;   /* track the number of nodes that have unallocated memory */
    
    struct fl_node *visited;    /* store the previously visited node ,used for next fit */
    struct fl_node *freelist;
    struct bu_node *bm_head;
    int bu_depth;
    int page_size;
} handlers[MAX_HANDLE];

void *ff_allot(struct handle *handlers, int handleCount, long n_bytes);
void *nf_allot(struct handle *handlers, int handleCount, long n_bytes);
void *bf_allot(struct handle *handlers, int handleCount, long n_bytes);
void *wf_allot(struct handle *handlers, int handleCount, long n_bytes);
void *rf_allot(struct handle *handlers, int handleCount, long n_bytes);
int meminit(long n_bytes, unsigned int flags, int parm1);
void *memalloc(int handle, long n_bytes);
void memfree(void* region);
void evaluate (int handle);

/*type 1 for change all its child to 1, 0 for change all children to 0 and 2 for only change one branch to 1*/
void modBitmap(int depth, struct bu_node *head,int index, int type);

int bu_free(struct handle this_handle, void *free_bytes);

int comp_pow(int num);
int find_parents(int num);
int find_gradchi(int num, int level);
int find_buddy(int num);


#endif

