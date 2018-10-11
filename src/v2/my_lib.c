#include <stdlib.h>
#include <stdlib.h>
#include "my_lib.h"

/*
    Meta:
        (integer)
        + if available capacity
        - if not available
*/

#define meta_t int
#define byte_t char
#define POOL_SIZE 2000

#define data_ref(v) ((void*)((void*)v + sizeof(meta_t))) // get data address from meta address
#define meta_ref(v) ((void*)((void*)v - sizeof(meta_t))) // get meta address from data address
#define is_free(v) ((meta_ref(v)>0)) // get if data address is free from meta

byte_t pool[POOL_SIZE]; 

static void initialize(){
    *(meta_t*)pool = POOL_SIZE - sizeof(meta_t); // set the available space
}

static void split(){

}

void * my_malloc(unsigned int size){
    if(!(meta_t*)pool) initialize();
    if(!size) return NULL;

    meta_t * tmp = (meta_t*)pool;
    while((byte_t *)tmp < pool+POOL_SIZE){
        if(*tmp >= size){
            if(*tmp > size) split();
            return data_ref(tmp);
        }
        if(*tmp > 0) tmp += *tmp;
        else tmp += -(*tmp);
    }

    return NULL;
}

int my_free(void * data_ref){
    return 0;
}

void print(){

}