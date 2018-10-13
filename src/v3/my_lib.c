#include <stdlib.h>
#include <stdio.h>
#include "my_lib.h"

// first fit with a binary search tree

typedef char bool_t;
typedef char byte_t;

typedef struct meta_t {
    struct meta_t * left; // left child
    struct meta_t * right; // right child
    struct meta_t * prev; // previous in row
    struct meta_t * next; // next in row
    unsigned int size; // size of the data block
    bool_t free;
} meta_t;

#define rep(a,b) for(int a=0;a<b;a++)
#define true 1
#define false 0

#define POOL_SIZE 1000 // additional meta space left at the end of pool

#define get_data_ref(v) ((void*)((void*)v + sizeof(meta_t))) // get data address from meta address
#define get_meta_ref(v) ((meta_t*)((void*)v - sizeof(meta_t))) // get meta address from data address


// Data Block:      [initialized byte] [ref to root of binary tree meta_t *] ... [meta_t][data] ...
static byte_t pool[POOL_SIZE]; 

static void print_bst_node(meta_t **ref, int tabs){
    if(!*ref) return;
    rep(t, tabs) printf("\t");
    printf("%p (size: %u, free: %d)\n", *ref, (*ref)->size, (*ref)->free);
    print_bst_node(&(*ref)->left, tabs + 2);
    print_bst_node(&(*ref)->right, tabs + 2);
}

static void initialize(){
    *(pool) = 1; // set the initialized byte to 1
    *(meta_t**)(pool+1) = (meta_t *)(pool + sizeof(meta_t*) + sizeof(byte_t));
    meta_t * ref = *(meta_t **)(pool+1);
    ref -> left = ref -> right = ref -> prev = ref -> next = NULL;
    ref -> size = POOL_SIZE - sizeof(meta_t *) - sizeof(meta_t) - sizeof(byte_t);
    ref -> free = true;
}

static int split(meta_t ** ref, meta_t ** new, const unsigned int size){
    if((*ref)->size <= size + sizeof(meta_t)) return -1; // cannot split. not enough space for new one
    (*new) = (meta_t *)((byte_t *)((*ref)+1) + size);
    (*new) -> free = true;
    (*new) -> size = (*ref) -> size - size - sizeof(meta_t);
    (*new) -> left = (*new) -> right = NULL;
    (*new) -> prev = (*ref);
    (*new) -> next = (*ref) -> next;
    
    (*ref) -> size = size;
    (*ref) -> next = (*new);
    return 0;
}

static int merge_with_next(meta_t *ref){
    if(!ref) return -1; // NULL value passed
    if(!ref->next) return -1; // there's no next
    
    meta_t * next = ref -> next;
    ref -> size = next -> size + sizeof(meta_t);
    return 0;
}

static meta_t ** get_fit_from_bst(meta_t ** ref, const unsigned int size){
    while((*ref) && (*ref) -> size < size) ref = &((*ref)->right);
    return ref;
}

static int remove_from_bst(meta_t ** ref, meta_t ** target){
    meta_t ** prev = ref;
    while(*ref && ref != target){
        prev = ref;
        if((*target)->size < (*ref)->size) ref = &((*ref) -> left);
        else ref = &((*ref) -> right);
    }
    if(ref != target) return -1; // could not find the container that holds the element to be removed
    if((*prev)->left == *target){
        // target is a left child
        if(!(*target)->right){
            *target = (*target) -> left;
            return 0;
        }
        while((*ref) -> right) ref = &((*ref) -> right);
        
    } else {
        // target is a right child
        if(!(*target)->left){
            *target = (*target) -> right;
            return 0;
        }
        while((*ref) -> left) ref = &((*ref) -> left);
    }

    if(remove_from_bst(target, ref)){ // remove the right most child/ left most child from bst
        *target = *ref;
        return 0;
    }
    return -1;
}

static int add_to_bst(meta_t **ref, meta_t * target){
    if(*ref == NULL){
        *ref = target;
        return 0;
    }

    if(target -> size < (*ref) -> size ) return add_to_bst(&((*ref)->left), target);
    return add_to_bst(&((*ref)->right), target);
}

void * my_malloc(const unsigned int size){
    if(!*pool) initialize();
    if(!size) return NULL;
    
    meta_t ** ref = (meta_t **)(pool+1);
    ref = get_fit_from_bst(ref, size);
    if(*ref == NULL) return NULL; // there are no items on the BST

    meta_t * current = *ref;
    meta_t * new = NULL;

    int res;
    res = remove_from_bst((meta_t **)(pool+1), ref);
    printf("[my_alloc] remove_from_bst result = %d\n", res);
    res = split(&current, &new, size);
    printf("[my_alloc] split result = %d, current %p, new %p\n", res, current, new);

    if(new) add_to_bst((meta_t **)(pool+1), new);
    current -> free = false;
    return get_data_ref(current);
}

int my_free(const void * data_ref){
    meta_t * ref = get_meta_ref(data_ref);
    if(ref -> free) return -1;
    ref -> free = true;

    int res = 0;
    if(ref -> next && ref -> next -> free) {
        remove_from_bst((meta_t **)(pool+1), (ref -> next));
        res = merge_with_next(ref);
        printf("[my_free] merge_with_next next node result => %d\n", res);
    }
    if(ref -> prev && ref -> prev -> free){
        remove_from_bst((meta_t **)(pool+1), ref -> prev);
        ref = ref -> prev;
        res = merge_with_next(ref);
        printf("[my_free] merge_with_next prev node result => %d\n", res);
    }

    res = add_to_bst((meta_t **)(pool+1), ref);
    printf("[my_free] add to bst result => %d\n", res);
    return 0;
}


void print_heap(const unsigned int tabs){
    if(!*pool) initialize();
    
    rep(t,tabs) printf("\t");
    printf("==========Heap=========\n");
    rep(t,tabs) printf("\t");
    printf("Heap [%p : %p], BST Root %p\n\n", pool, pool+POOL_SIZE, *(meta_t**)(pool+1));

    meta_t * tmp = (meta_t*)(pool+sizeof(byte_t)+sizeof(meta_t *));
    while(tmp && (byte_t *)tmp < pool+POOL_SIZE){
        rep(t,tabs) printf("\t");
        printf("Meta: %p -> %9d, %d\t\t", tmp, tmp -> size, tmp -> free);
        printf("Data: %p -> %9d\n", get_data_ref(tmp), *(int *)get_data_ref(tmp));

        tmp = (meta_t *)((byte_t *)(tmp + 1) + tmp -> size);
    }

    rep(t, tabs) printf("\t");
    printf("BST ==>\n");
    print_bst_node((meta_t**)(pool+1), tabs);
}
