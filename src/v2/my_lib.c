#include <stdlib.h>
#include <stdio.h>
#include "my_lib.h"

#define meta_t int
#define byte_t char
#define POOL_SIZE 100

#define data_ref(v) ((void*)((void*)v + sizeof(meta_t))) // get data address from meta address

byte_t pool[POOL_SIZE]; 

static void initialize(){
    *(meta_t*)pool = POOL_SIZE - sizeof(meta_t); // set the available space

}

static void split(meta_t * ref, unsigned int size){
    if(*ref <= sizeof(meta_t) + size) return; // there's not enough space to split into two sections
    *(meta_t *)((byte_t *)ref + (sizeof(meta_t) + size)) = *ref - size - 2 * sizeof(meta_t); // set the meta tag size
    *ref = size;
}

void * my_malloc(unsigned int size){
    if(!*(meta_t*)pool) initialize();
    if(!size) return NULL;

    meta_t * tmp = (meta_t *)pool;
    while((byte_t *)tmp < pool+POOL_SIZE){
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

int my_free(void * data_ref){
    return 0;
}

void print(){
    printf("=======Heap=====\n");
    meta_t * tmp = (meta_t*)pool;
    while((byte_t *)tmp < pool+POOL_SIZE){
        printf("Meta: %p -> %d\n", tmp, *tmp);
        printf("Data: %p -> %d\n\n", data_ref(tmp), *(int *)data_ref(tmp));

        if(*tmp > 0) tmp = (meta_t *)((byte_t *)(tmp) + (*tmp + sizeof(meta_t)));
        else tmp = (meta_t *)((byte_t *)(tmp) + (sizeof(meta_t) - (*tmp)));
    }
}