#include <stdio.h>
#include <stdlib.h>
#include "v2/my_lib.h"

int main(){
    int N;
    scanf("%d", &N); // number of memory slots to be allocated

    int * points[N];
    int freed[N];
    for(int i=0;i<N;i++){
        points[i] = (int *)my_malloc(8);
        if(!points[i]) {
            printf("Out of memory\n");
            print();
            exit(0);
        }
        *points[i] = i*i;
        freed[i] = 0;
        printf("\t\tResponse: %p -> %d\n", points[i], *points[i]);
    }

    print();
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