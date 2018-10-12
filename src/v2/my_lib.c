#include <stdlib.h>
#include <stdio.h>
#include "my_lib.h"

#define meta_t int
#define byte_t char

#define POOL_SIZE 100 // additional meta space left at the end of pool

#define rep(a,b) for(int a=0;a<b;a++)
#define get_data_ref(v) ((void*)((void*)v + sizeof(meta_t))) // get data address from meta address
#define get_meta_ref(v) ((void*)((void*)v - sizeof(meta_t))) // get meta address from data address

// Data Block:      ... [meta_t][data][meta_t] ...
// meta_t:          (+)ve if free

static byte_t pool[POOL_SIZE]; 

static void initialize(){
    *((meta_t*)pool) = POOL_SIZE - 2*sizeof(meta_t); // set the available space
    // *((meta_t*)()) = *((meta_t*)pool);
}

static void split(meta_t * ref, const unsigned int size){
    if(*ref <= sizeof(meta_t) + size) return; // there's not enough space to split into two sections
    *(meta_t *)((byte_t *)ref + (sizeof(meta_t) + size)) = *ref - size - sizeof(meta_t); // set the meta tag size
    *ref = size; // update size of the memory block
}

static void merge_with_next(meta_t * ref){
    *ref += *(meta_t *)((byte_t *)(ref) + *ref) + sizeof(meta_t);
}

void * my_malloc(const unsigned int size){
    if(!*(meta_t*)pool) initialize();
    if(!size) return NULL;

    meta_t * tmp = (meta_t *)pool;
    while(*tmp && (byte_t *)tmp < pool+POOL_SIZE){
        if(*tmp > 0 && *tmp >= size){
            if(*tmp > size) split(tmp, size);
            *tmp *= -1; // make the space as allocated
            return get_data_ref(tmp);
        }

        if(*tmp > 0) tmp = (meta_t *)((byte_t *)(tmp) + (*tmp + sizeof(meta_t)));
        else tmp = (meta_t *)((byte_t *)(tmp) + (sizeof(meta_t) - (*tmp)));
    }

    return NULL;
}

int my_free(const void * data_ref){
    meta_t * ref = get_meta_ref(data_ref);
    if((byte_t *)(ref) < pool || (byte_t *)(ref) >= pool + POOL_SIZE) return -1; // invalid address
    if(*ref >= 0) return -1; // already freed data / invalid data
    *ref *= -1; // mark the block as freed

    meta_t * next = (meta_t *)((byte_t *)(ref) + (*ref + sizeof(meta_t)));
    if(*next && (byte_t *)next < pool + POOL_SIZE && *next > 0) merge_with_next(ref);

    // prev function fails. utter failure. v2 halts here.
    return 0;
}

void print_heap(const unsigned int tabs){
    if(!*(meta_t*)pool) initialize();
    
    rep(t,tabs) printf("\t");
    printf("==========Heap=========\n");
    rep(t,tabs) printf("\t");
    printf("Heap [%p : %p]\n\n", pool, pool+POOL_SIZE);

    meta_t * tmp = (meta_t*)pool;
    while(*tmp && (byte_t *)tmp < pool+POOL_SIZE){
        rep(t,tabs) printf("\t");
        printf("Meta: %p -> %9d\t\t", tmp, *tmp);
        printf("Data: %p -> %9d\n", get_data_ref(tmp), *(int *)get_data_ref(tmp));

        if(*tmp > 0) tmp = (meta_t *)((byte_t *)(tmp) + (*tmp + sizeof(meta_t)));
        else tmp = (meta_t *)((byte_t *)(tmp) + (sizeof(meta_t) - (*tmp)));
    }
}