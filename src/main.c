#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "my_lib.h"

#define rep(a,b) for(int a=0;a<b;a++)
#define index(a) ((a+10)*(a+10))
#define MX 1000

typedef char bool;

#define true 1
#define false 0

int N, M;

int * points[MX];
int sizes[MX];

bool allocated[MX];
bool freed[MX];

int allocates;
int frees;

void free_memory(){
    while(1){
        int index = rand() % N;
        if(freed[index] || !allocated[index]) continue;
        printf("Freeing %d -> %d\n", index(index), my_free(points[index]));
        // print_heap(2);
        printf("Freed %d.\n\n\n", index(index));
        freed[index] = 1;
        break;
    }
}

void allocate_memory(){
    int size = rand() % M + 1;
    sizes[allocates] = size;
    printf("Index: %d\n", index(allocates));
    printf("Allocating array of integers of length %d (%lu bytes)\n", size, sizeof(int) * size);
    points[allocates] = (int *)my_malloc(sizeof(int) * size);

    if(!points[allocates]) {
        printf("Out of memory\n");
        print_heap(2);
        exit(0);
    }

    rep(x,sizes[allocates]) points[allocates][x] = index(allocates);
    freed[allocates] = 0;

    printf("Allocated: %p -> %d\n", points[allocates], points[allocates][0]);
    // print_heap(2);
    printf("\n\n");
    allocated[allocates] = true;
}

int main(){
    srand(time(NULL));
    scanf("%d %d", &N, &M); // number of memory slots to be allocated, max test array size
    print_heap(2);
    printf("\n\n");

    rep(i,2*N){
        bool allocate = rand() % 2;
        if(allocate && allocates >= N) allocate = false;
        if(!allocate && (allocates-frees) <= 0) allocate = true;
        
        if(allocate){
            allocate_memory();
            allocates++;
        } else {
            free_memory();
            frees++;
        }
    }

    print_heap(2);

    printf("\n");
    printf("ALLOCATES: %d, FREES: %d\n", allocates, frees);
    return 0;
}