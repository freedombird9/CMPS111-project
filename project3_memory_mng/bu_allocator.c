#include <stdio.h>
#include <stdlib.h>
#include "libmem.h"


void modBitmap(int depth, struct bu_node *root, int used_val);


void *buddy_allot(struct handle *this_handle, unsigned long alot_bytes){
    int level;      /*the max possible depth of the allocated block*/
    /*allot the whole struct*/
    if(alot_bytes==this_handle->n_bytes){
        modBitmap(this_handle->bu_depth, this_handle->bitmap, 1);
        return this_handle->memstart;
    }
    if((alot_bytes % this_handle->page_size)==0)
        level=this_handle->bu_depth-(alot_bytes/this_handle->page_size)/2;
    else
        level=this_handle->bu_depth-(alot_bytes/this_handle->page_size+1)/2;



}

void modBitmap(int depth, struct bu_node *root, int used_val){
  root->used = used_val;
  while(depth > 0){
    depth=depth-1;
    modBitmap(depth, root->left,used_val);
    modBitmap(depth, root->right,used_val);
  }
}
