#include <stdio.h>
#include <stdlib.h>
#include "libmem.h"


void modBitmap(int depth, struct bu_node *root, int used_val);


void *buddy_allot(struct handle *this_handle, unsigned long alot_bytes){
    int level;      /*the max possible depth of the allocated block*/
    struct bu_node *walk;
    int flag_found=0; /**/
    /*allot the whole struct*/
    if(alot_bytes==this_handle->n_bytes){
        modBitmap(this_handle->bu_depth, this_handle->bitmap, 1);
        return this_handle->memstart;
    }
    /*the allocated part is small than the whole part*/
    /*compute the level*/
    if((alot_bytes % this_handle->page_size)==0)
        level=this_handle->bu_depth-(alot_bytes/this_handle->page_size)/2;
    else
        level=this_handle->bu_depth-(alot_bytes/this_handle->page_size+1)/2;
    /*traversal the tree to find the fit block*/
    walk=this_handle->bitmap;
    while(level>0){
        if(walk->used==1){
            if(level>2){
                walk=walk->left;
                level=level-1;
                continue;
            }
            /*the block that the body is allocated but is is not*/
            if(level==2){
                if(((walk->left->used==0)&&(walk->right->used==1))||((walk->left->used==0)&&(walk->right->used==1))){
                    walk->right->used=1;
                    flag_found=1;
                    return walk->right->pointer;
                }
            }

        }
                        level=level-1;
        }

}

void modBitmap(int depth, struct bu_node *root, int used_val){
  root->used = used_val;
  while(depth > 0){
    depth=depth-1;
    modBitmap(depth, root->left,used_val);
    modBitmap(depth, root->right,used_val);
  }
}
