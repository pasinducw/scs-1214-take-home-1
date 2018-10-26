#include <stdio.h>
#include <stdlib.h>
#include "../my_lib.h"

// Allocates memory using first fit algorithm 
// (first fit algo = implmented in first semester for DSA take home assignment)

typedef struct memory_slot {
    int free;
    int size;
    struct memory_slot * prev;
    struct memory_slot * next;
} memory_slot;

#define data_ref(pt) ((void *)((void *)pt + sizeof(memory_slot)))
#define POOL_SIZE 25000

static unsigned char memory_pool[POOL_SIZE];
static memory_slot * root;
static memory_slot * ptr;
static memory_slot * tmp;

static void initialize(){
    root = (memory_slot *)memory_pool;
    root -> free = 1;
    root -> size = POOL_SIZE - sizeof(memory_slot);
    root -> prev = NULL;
    root -> next = NULL;
}

static int split(memory_slot * slot, unsigned int size){
    tmp = (memory_slot *)((void *)slot + sizeof(memory_slot) + size);
    if((void *)(tmp+1) >= (void *)(memory_pool+POOL_SIZE)) { // not enough memory for a split
        return -1;
    }
    tmp -> free = 1;
    tmp -> size = slot -> size - size - sizeof(memory_slot);
    tmp -> next = slot -> next;
    tmp -> prev = slot;

    if(slot -> next) slot -> next -> prev = tmp;
    slot -> next = tmp;
    slot -> size = size;
    return 0;
}

static void combine_next(memory_slot * ptr){
    ptr -> size += ptr -> next -> size + sizeof(memory_slot);
    ptr -> next = ptr -> next -> next;
    if(ptr -> next) ptr -> next -> prev = ptr;
}

void * my_malloc(unsigned int size) {
    if(!root) initialize(); // initialized structure on first call
    if(!size) return NULL; // invalid size (0)
    
    ptr = root;
    while(ptr!=NULL){ // loop through the meta blocks and find a fitting free block
        if(ptr->free && ptr->size >= size){
            if(ptr->size > size) split(ptr, size); // split block if more size is available
            ptr -> free = 0;
            return data_ref(ptr);
        }
        ptr = ptr -> next;
    }
    
    // no space available
    return NULL;
}

// returns 0 upon successful call
int my_free(const void * data_location){
    if(!root) return -1; // invalid call

    ptr = root;
    while(ptr != NULL){
        if(data_ref(ptr) == data_location){
            ptr -> free = 1;
            if(ptr -> next && ptr -> next -> free) combine_next(ptr);
            if(ptr -> prev && ptr -> prev -> free) {
                ptr = ptr -> prev;
                combine_next(ptr);
            }
            return 0;
        }
        ptr = ptr -> next;
    }
    return -1;
}

// utility function to see the current memory condition
void print_heap(const unsigned int tabs){
    if(!root) initialize(); // initialized structure on first call
    printf("=====My Heap=====\n");
    ptr = root;
    while(ptr != NULL){
        printf("Meta: %p\t(%d, %d, %p, %p)\n", ptr, ptr->free, ptr->size, ptr->prev, ptr->next);
        printf("Data: %p\n", data_ref(ptr));
        ptr = ptr -> next;
    }
}