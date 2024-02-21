#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

static double run50T(void (*task)()) {
    double time = 0;
    clock_t start, end;

    for(int i = 0; i < 50; i++){
        start = clock();
        (*task)();
        end = clock();
        time += ((double)end - start) / (CLOCKS_PER_SEC);
    }
    return time/50;
}

//Performance Test 1;
static void oneByte120T (){
    char *p;
    for(int i = 1; i <= 120; i++){
        p =(char*)malloc(1);
        free(p);
    }
}

//Performance Test 2;
static void arrPtr1Byte120T (){
    char* ptrArr[120];
    for(int i = 1; i <= 120; i++){
        ptrArr[i-1] = (char*)malloc(1);
    }
    for(int j = 1; j <= 120; j++){
        free(ptrArr[j-1]);
    }
}

//Performance Test 3;
static void randomMalloc1Byte120T (){
    srand(time(NULL));
    char* ptrArr[120];
    int allocate = 0, counter = 0, allocations = 0, zeroAlloc = 0;
    while (allocations < 120){
        allocate = rand()%2;
        if (allocate == 1){
            ptrArr[counter] = (char*)malloc(1);
            if (counter == 0){
                zeroAlloc = 1;
            }
            allocations++;
            counter++;
        }
        else{
            if(counter == 1 && zeroAlloc == 1){
                free(ptrArr[counter-1]);
                zeroAlloc = 0;
                counter--;
            }
            if(counter > 0){
                free(ptrArr[counter-1]);
                counter--;
            }
        }
    }
    for (int i = 0; i < counter; i++){
        if(i == 0 && zeroAlloc == 1){
            free(ptrArr[i]);
        }
        if (i > 0){
            free(ptrArr[i]);
        }
    }
}

//Performance Test 4;
static void reverseFree20Byte120T(){
    char* ptrArr[120];
    for(int i = 0; i < 120; i++){
        ptrArr[i] = (char*)malloc(10);
    }
    for(int j = 119; j >= 0; j--){
        free(ptrArr[j]);
    }

    char* ptr = (char*)malloc(4080);
    free(ptr);
}

//Performance Test 5;
static void randomByteAllocation120T(){
    srand(time(NULL));
    char* ptrArr[120];
    int allocate = 0, counter = 0, allocations = 0, zeroAlloc = 0, randomByte = 0;
    while (allocations < 120){
        allocate = rand()%2;
        if (allocate == 1){
            randomByte = (rand()%10)+1;
            ptrArr[counter] = (char*)malloc(randomByte);
            if (counter == 0){
                zeroAlloc = 1;
            }
            allocations++;
            counter++;
        }
        else{
            if(counter == 1 && zeroAlloc == 1){
                free(ptrArr[counter-1]);
                zeroAlloc = 0;
                counter--;
            }
            if(counter > 0){
                free(ptrArr[counter-1]);
                counter--;
            }
        }
    }
    for (int i = 0; i < counter; i++){
        if(i == 0 && zeroAlloc == 1){
            free(ptrArr[i]);
        }
        if (i > 0){
            free(ptrArr[i]);
        }
    }

}

//main func to excuete all the tests
int main (){
    printf("AVG TIME FOR MALLOC/FREE 1 BYTE 120 TIMES: %lf seconds\n", run50T(oneByte120T));
    printf("AVG TIME FOR MALLOC/FREE ARRAY OF 120 PTR OF 1 BYTE: %lf seconds\n", run50T(arrPtr1Byte120T));
    printf("AVG TIME FOR RANDOM MALLOC/FREE ARRAY OF 120 PTR OF 1 BYTE: %lf seconds\n", run50T(randomMalloc1Byte120T));
    printf("AVG TIME FOR REVERSE FREEING MEMORY OF 10 BYTES AND ALLOCATING WHOLE CHUNK TO SEE COALESCE: %lf seconds\n", run50T(reverseFree20Byte120T));
    printf("AVG TIME FOR RANDOM MALLOC/FREE ARRAY OF 120 PTR OF RANDOM BYTES: %lf seconds\n", run50T(randomByteAllocation120T));
}