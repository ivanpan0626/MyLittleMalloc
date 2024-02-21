#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mymalloc.h"

#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

/*TEST CASE 1, */
//checks to see if malloc correctly allocates space in the memory
static void mallocReserveUnallocMem(){
    int* p = malloc(sizeof(double)*2);
    int* q = malloc(sizeof(double)*2);
    if(p != NULL && q != NULL){
        printf("TEST CASE 1: PASSED\n");
    }
    free(p);
    free(q);
    return;
}

/*TEST CASE 2,*/
//tests for ptr overlap during mallocs to see if the ptr to headers are correctly being assigned
static void mallocPtrOverlap(){
    int *p = malloc(sizeof(int));
    int *q = malloc(sizeof(int));

    if (p >= q-1 && p <= q) {
        printf("TEST CASE 2: FAILED, overlapped memory\n");
        return;
    } else {
        printf("TEST CASE 2: PASSED\n");
    }
    free(p);
    free(q);
    return;
}

/*TEST CASE 3,*/
//tests for distinct bytes and that no bytes are being overwritten, credits: memTest.c
static void mallocDistinctBytes(){
    char *obj[OBJECTS];
	int i, j, errors = 0;
	
	// fill memory with objects
	for (i = 0; i < OBJECTS; i++) {
		obj[i] = malloc(OBJSIZE);
	}
	
	// fill each object with distinct bytes
	for (i = 0; i < OBJECTS; i++) {
		memset(obj[i], i, OBJSIZE);
	}
	
	// check that all objects contain the correct bytes
	for (i = 0; i < OBJECTS; i++) {
		for (j = 0; j < OBJSIZE; j++) {
			if (obj[i][j] != i) {
				errors++;
			}
		}
	}
    if (errors>0){
        printf("TEST CASE 3: FAILED, error bytes: %d\n", errors);
    }
    else{
        printf("TEST CASE 3: PASSED\n");
    }
    for (i = 0; i < OBJECTS; i++) {
		free(obj[i]);
	}

}

/*TEST CASE 4,*/
//tests both free is functioning correctly and whether coalesces is being performed as memory is being freed in random order
static void free_coalesced(){
    int *p = malloc(sizeof(char)*2000);
    int *q = malloc(sizeof(char)*200);
    int *r = malloc(sizeof(char)*600);
    int *s = malloc(sizeof(char)*800);
    free(s);
    free(q);
    free(r);
    free(p);
    int *v = malloc(sizeof(char)*4088);
    if (v == NULL){
        printf("TEST CASE 4: FAILED, memory not coalesced\n");
        return;
    }
    else {
        printf("TEST CASE 4: PASSED\n");
    }
    free(v);
    return;
}

/*TEST CASE 5*/
//tests to see if it will allocate the last 8 byte (impossible), since 16 bytes are needed to allocate 8 for memory and 8 for header
static void endOfMemory(){
    int *p = malloc(sizeof(char)*4080);
    printf("Should produce not enough memory error since only 8 bytes left: "); //Specifically for 4096, need to recalculate math for other sizes
    int *q = malloc(sizeof(char)*8);
    if (q == NULL){
        printf("TEST CASE 5: PASSED\n");
    }
    else{
        printf("TEST CASE 5: FAILED, should not be able to allocate last 8 bytes, need MIN: 16 bytes of memory region left\n");
    }
    free(p);
}

/*TEST CASE 6*/
//Tests to see if 8 byte alignment is performed when requested size is not divisible by 8
static void corrAlignment(){
    int *p = malloc(3980);
    printf("Should produce error message cause of 3980 aligns to 3984, rendering 100 too big: ");//Specifically for 4096
    int *q = malloc(100); //this should fail
    int *r = malloc(88); //should work
    if (q == NULL && r != NULL){
         printf("TEST CASE 6: PASSED\n");
    }
    free(p);
    free(r);
}

static void detectableErrors (){
    //checks for free thats not in memory region
    int p;
    free(&p);

    //checks for freeing ptr not at start of chunk
    int *q = malloc(sizeof(int)*2);
    free(q + 1);

    //Checks for double free eror
    int *r = malloc(sizeof(int)*100);
    int *s = r;
    free(r);
    free(s);

    int *t = malloc(0); //checks for malloc(0) edge case
    int *v = malloc(4097); //checks for malloc() that exceeds biggest size


}

int main (){
    printf("------CORRECTNESS TESTING------\n");
    mallocReserveUnallocMem();
    mallocPtrOverlap();
    mallocDistinctBytes();
    free_coalesced();
    endOfMemory();
    corrAlignment();
    printf("--------------END--------------\n");
    printf("---Detectable Error Checking--- \n");
    detectableErrors();
    printf("--------------END--------------\n");
    return 0;
}
