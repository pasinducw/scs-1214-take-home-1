#include <stdio.h>
#include <stdlib.h>
#include "v2/my_lib.h"

#define rep(a,b) for(int a=0;a<b;a++)
#define index(a) ((a+10)*(a+10))

int main(){
    int N, M;
    scanf("%d %d", &N, &M); // number of memory slots to be allocated, max test array size

    int * points[N];
    int sizes[N];
    int freed[N];
    
    print_heap(2);
    printf("\n\n");
    rep(i,N){
        // determinig array size to be allocated
        sizes[i] = rand() % M + 1;

        printf("Index: %d\n", index(i));
        printf("Allocating array of integers of length %d (%lu bytes)\n", sizes[i], sizeof(int) * sizes[i]);
        points[i] = (int *)my_malloc(sizeof(int) * sizes[i]);
    
        if(!points[i]) {
            printf("Out of memory\n");
            print_heap(2);
            exit(0);
        }

        rep(x,sizes[i]) points[i][x] = index(i);
        freed[i] = 0;

        printf("Allocated: %p -> %d\n", points[i], points[i][0]);
        print_heap(2);
        printf("\n\n");
    }
/*
    int freeCount = 0;
    while(freeCount<N){
        int index = rand() % N;
        if(freed[index]) continue;
        printf("Freeing %d -> %d\n", index, my_free(points[index]));
        print();
        freed[index] = 1;
        freeCount++;
    }
*/
    return 0;
}