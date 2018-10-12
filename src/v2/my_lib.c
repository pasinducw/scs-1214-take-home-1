#include <stdlib.h>
#include <stdio.h>
#include "my_lib.h"

#define meta_t int
#define byte_t char
#define POOL_SIZE 100

#define rep(a,b) for(int a=0;a<b;a++)
#define data_ref(v) ((void*)((void*)v + sizeof(meta_t))) // get data address from meta address

byte_t pool[POOL_SIZE]; 

static void initialize(){
    *(meta_t*)pool = POOL_SIZE - sizeof(meta_t); // set the available space
}

static void split(meta_t * ref, const unsigned int size){
    if(*ref <= sizeof(meta_t) + size) return; // there's not enough space to split into two sections
    *(meta_t *)((byte_t *)ref + (sizeof(meta_t) + size)) = *ref - size - sizeof(meta_t); // set the meta tag size
    *ref = size; // update size of the memory block
}

void * my_malloc(const unsigned int size){
    if(!*(meta_t*)pool) initialize();
    if(!size) return NULL;

    meta_t * tmp = (meta_t *)pool;
    while(*tmp && (byte_t *)tmp < pool+POOL_SIZE){
        if(*tmp > 0 && *tmp >= size){
            if(*tmp > size) split(tmp, size);
            *tmp *= -1; // make the space as allocated
            return data_ref(tmp);
        }

        if(*tmp > 0) tmp = (meta_t *)((byte_t *)(tmp) + (*tmp + sizeof(meta_t)));
        else tmp = (meta_t *)((byte_t *)(tmp) + (sizeof(meta_t) - (*tmp)));
    }

    return NULL;
}

int my_free(const void * data_ref){
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
        printf("Data: %p -> %9d\n", data_ref(tmp), *(int *)data_ref(tmp));

        if(*tmp > 0) tmp = (meta_t *)((byte_t *)(tmp) + (*tmp + sizeof(meta_t)));
        else tmp = (meta_t *)((byte_t *)(tmp) + (sizeof(meta_t) - (*tmp)));
    }
}