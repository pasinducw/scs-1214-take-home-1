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

#define POOL_SIZE 25000 // additional meta space left at the end of pool

#define get_data_ref(v) ((void*)((void*)v + sizeof(meta_t))) // get data address from meta address
#define get_meta_ref(v) ((meta_t*)((void*)v - sizeof(meta_t))) // get meta address from data address


// Data Block:      [initialized byte] [ref to root of binary tree meta_t *] ... [meta_t][data] ...
static byte_t pool[POOL_SIZE]; 

static void print_bst_node(meta_t *ref, int tabs){
    if(!ref) return;
    rep(t, tabs) printf("\t");
    printf("%p (size: %u, free: %d)\n", ref, ref->size, ref->free);
    print_bst_node(ref->left, tabs + 2);
    print_bst_node(ref->right, tabs + 2);
}

static void initialize(){
    *(pool) = 1; // set the initialized byte to 1
    *(meta_t**)(pool+1) = (meta_t *)((pool) + sizeof(meta_t*) + sizeof(byte_t));
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

    if((*ref) -> next) (*ref) -> next -> prev = (*new);
    
    (*ref) -> size = size;
    (*ref) -> next = (*new);
    return 0;
}

static int merge_with_next(meta_t *ref){
    if(!ref) return -1; // NULL value passed
    if(!ref->next) return -1; // there's no next
    
    meta_t * next = ref -> next;
    ref -> size += next -> size + sizeof(meta_t);
    ref -> next = next -> next;
    if(next -> next) next -> next -> prev = ref;
    return 0;
}

static meta_t * get_fit_from_bst(meta_t * ref, const unsigned int size){
    // get the best fit
    meta_t * best_ref = NULL;
    while(ref){
        if(ref -> size >= size) best_ref = ref;
        if(size < ref -> size) ref = ref -> left;
        else ref = ref -> right;
    }
    return best_ref;
}

static int remove_from_bst(meta_t ** ref, meta_t * target){
    // printf("Remove from BST. rooted at %p. target %p\n", *ref, target);
    while(*ref && *ref != target){
        if(target->size < (*ref)->size) ref = &((*ref) -> left);
        else ref = &((*ref) -> right);
    }
    if(*ref != target) {
        return -1;
    }

    if(!target->right && !target->left){ // no child nodes
        *ref = NULL;
        return 0;
    }
    if(target->right && !target->left){ // has only right child
        *ref = target->right;
        target -> left = target -> right = NULL;
        return 0;
    }
    if(target->left && !target->right){ // has only left child
        *ref = target->left;
        target -> left = target -> right = NULL;
        return 0;
    }
    // has both children
    meta_t * tmp = target -> right;
    while(tmp -> left) tmp = tmp->left;
    if(!remove_from_bst(ref, tmp)){ // remove 
        if(tmp != target -> right) tmp -> right = target -> right;
        tmp -> left = target -> left;
        *ref = tmp;
        target -> left = target -> right = NULL;
        return 0;
    }
    return -1;
}

static int add_to_bst(meta_t **ref, meta_t * target){
    if(*ref == NULL){
        *ref = target;
        target -> left = target -> right = NULL;
        return 0;
    }

    if(target -> size < (*ref) -> size ) return add_to_bst(&((*ref)->left), target);
    return add_to_bst(&((*ref)->right), target);
}

void * my_malloc(const unsigned int size){
    if(!*pool) initialize();
    if(!size) return NULL;
    
    meta_t * ref = get_fit_from_bst(*(meta_t **)(pool+1), size);
    if(!ref) return NULL; // there are no items on the BST

    meta_t * current = ref;
    meta_t * new = NULL;

    int res;
    res = remove_from_bst((meta_t **)(pool+1), ref);
    // printf("[my_alloc] remove_from_bst result = %d\n", res);
    res = split(&current, &new, size);
    // printf("[my_alloc] split result = %d, current %p, new %p\n", res, current, new);

    if(new) add_to_bst((meta_t **)(pool+1), new);
    current -> free = false;
    return get_data_ref(current);
}

int my_free(const void * data_ref){
    meta_t * ref = get_meta_ref(data_ref);
    if(ref -> free) return -1;
    ref -> free = true;

    int res;
    if(ref -> next && ref -> next -> free) {
        res = remove_from_bst((meta_t **)(pool+1), ref -> next);
        // printf("[my_free] remove_from_bst next node result => %d\n", res);
        res = merge_with_next(ref);
        // printf("[my_free] merge_with_next next node result => %d\n", res);
    }
    if(ref -> prev && ref -> prev -> free){
        res = remove_from_bst((meta_t **)(pool+1), ref -> prev);
        // printf("[my_free] remove_from_bst prev node result => %d\n", res);
        ref = ref -> prev;
        res = merge_with_next(ref);
        // printf("[my_free] merge_with_next prev node result => %d\n", res);
    }

    res = add_to_bst((meta_t **)(pool+1), ref);
    // printf("[my_free] add to bst result => %d\n", res);
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
        printf("Data: %p -> %9d \t\t", get_data_ref(tmp), *(int *)get_data_ref(tmp));
        printf("(left: %p, right: %p, prev: %p, next: %p\n", tmp -> left, tmp -> right, tmp -> prev, tmp -> next);

        tmp = (meta_t *)((byte_t *)(tmp + 1) + tmp -> size);
    }

    rep(t, tabs) printf("\t");
    printf("BST ==>\n");
    print_bst_node(*(meta_t**)(pool+1), tabs);
}
