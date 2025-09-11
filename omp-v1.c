#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define SIZE 10

int main(int argc, char *argv[])
{
    int v1[SIZE];
    int v2[SIZE];
    int matriz[SIZE][SIZE];

    for (int i=0; i<SIZE; i++){
        v1[i] = i;
        for (int j=0; j<SIZE; j++){
            matriz[i][j] = i+j;
        }
    }

    #pragma omp parallel
    {
        #pragma omp for
        for (int i=0; i<SIZE; i++){
            for(int j=0; j<SIZE; j++){
                v2[i] += matriz[i][j]*v1[j];
            }
        }   
    }

    for (int i=0; i<SIZE; i++){
        printf("%d " , v2[i]);
    }
    return 0;
}

