#include <stdio.h>
#include <stdlib.h>
#include "v3/my_lib.h"

#define rep(a,b) for(int a=0;a<b;a++)
#define index(a) ((a+10)*(a+10))
#define MX 1000

int N, M;
int * points[MX];

int sizes[MX];
int allocated[MX];
int freed[MX];

int currentSize;
int allocateCalls;

void free_memory(){
    while(1){
        int index = rand() % currentSize;
        if(freed[index] || !allocated[index]) continue;
        printf("Freeing %d -> %d\n", index, my_free(points[index]));
        print_heap(2);
        printf("Freed.\n");
        freed[index] = 1;
        break;
    }
}

void allocate_memory(){
    int size = rand() % M + 1;
    sizes[currentSize] = size;
    printf("Index: %d\n", index(allocateCalls));
    printf("Allocating array of integers of length %d (%lu bytes)\n", size, sizeof(int) * size);
    points[currentSize] = (int *)my_malloc(sizeof(int) * size);

    if(!points[currentSize]) {
        printf("Out of memory\n");
        print_heap(2);
        exit(0);
    }

    rep(x,sizes[currentSize]) points[currentSize][x] = index(currentSize);
    freed[currentSize] = 0;

    printf("Allocated: %p -> %d\n", points[currentSize], points[currentSize][0]);
    print_heap(2);
    printf("\n\n");
    allocated[currentSize] = 1;
    currentSize++; allocateCalls++;
}

int main(){
    scanf("%d %d", &N, &M); // number of memory slots to be allocated, max test array size
    print_heap(2);
    printf("\n\n");

    int availableToFree = 0;
    rep(i,2*N){
        int allocate = rand() % 2;
        if(!allocate && !availableToFree) allocate = 1;
        
        if(allocate){
            printf("Allocating\n");
            availableToFree++;
            allocate_memory();
        } else {
            printf("Freeing\n");
            availableToFree --;
            free_memory();
        }
    }
    return 0;
}