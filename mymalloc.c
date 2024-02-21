#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

//Macros
#define MEMLENGTH 512 
#define HEAD_SIZE sizeof(memData)
#define ALIGNBYTES(x) (((x) + 7) & (-8))
#define MAXMEMBYTES (MEMLENGTH*sizeof(double)) - HEAD_SIZE
#define MINBYTE 0

static double memory[MEMLENGTH];

//MetaData struct, memory data
typedef struct memoryData {
   unsigned int dsize; // datasize including HEAD_SIZE + PAYLOAD
   unsigned int allocated; //0 = free, 1 = allocated
} memData;

//Checks if current header is allocated
static int isAllocated(memData* header){
   if(header->allocated == 0){
      return 0; //not allocated
   }
   else {
      return 1; //allocated
   }
}

//returns the next header/metaData struct 
static memData* nextHeader(memData* header){
   return (memData*)((char*)header + header->dsize);
}

//Coalesce function, combines smaller memory chunks into one bigger one
static void combineSegments(){
   memData* curr_Header = (memData*)memory; //header at start of memory

   //loops to check for adjacent unallocated headers to combine with, (Checks left to right)
   while (curr_Header < (memData*)(memory + MEMLENGTH)) {
      if (curr_Header->allocated == 0){

         //makes sure the next header is not the end of memory region and is unallocated
         if (nextHeader(curr_Header) != (memData*)(memory + MEMLENGTH) && nextHeader(curr_Header)->allocated == 0) {
            curr_Header->dsize += nextHeader(curr_Header)->dsize; //combines the two headers size
         }
         else {
            nextHeader(curr_Header); //in the event next header is allocated, combineSegments() starts again from the next next header
            if(nextHeader(curr_Header) < (memData*)(memory + MEMLENGTH)){
               curr_Header = nextHeader(curr_Header);
            }
            else {
               return; //went past memory region, no more chunks to combine
            }
         }
      }
      else {
         curr_Header = nextHeader(curr_Header);
      }
   }
   return;
}

void *mymalloc(size_t size, char *file, int line) {
   memData* currHeader = (memData*)memory; // index pointer to iterate through memory, starts at memory[0]
   int og_size = (int)size; //var used to hold original size to report for oversized allocations
   
   //Checks if requested size exceeds total memory or is less than 0
   if (size > MAXMEMBYTES || size <= 0){
      printf("mymalloc, Memory out of bounds error: requested = 0 or too large: %s, line %d, malloc(%d) \n", file, line, og_size);
      return NULL;
   }
   size = ALIGNBYTES(size); //aligns size to multiples of 8 for alignment issues

   //Checks if chunk size <= 0,
   //if true, intializes memory array for memory allocation
   if (!(currHeader->dsize > 0)) {
      memData* initHeader = (memData *) memory;
      initHeader->dsize = MAXMEMBYTES + HEAD_SIZE; //sets the initial header->size to the max byte size
   }

   //Attempts to find a chunk that is unallocated and large enough for requested size
   while (currHeader < (memData*)(memory + MEMLENGTH)) {

      //Checks if chunk is free and large enough
      if ((currHeader->dsize - HEAD_SIZE) >= size && !(isAllocated(currHeader))) {

         //Compares the size of the chunk to requested size, 
         //in the event chunk is larger it can be split up for further uses
         if ((currHeader->dsize - HEAD_SIZE) > size) {
            char* nextHeaderAddr = (char*)currHeader;
            nextHeaderAddr = nextHeaderAddr + (size + HEAD_SIZE);
            memData* newHeader = (memData*)nextHeaderAddr;

            newHeader->dsize = currHeader->dsize - (size + HEAD_SIZE);
            newHeader->allocated = 0;

            currHeader->dsize = size + HEAD_SIZE;
         }
         currHeader->allocated = 1; //sets chunk to allocated
         return currHeader + 1;
      }
      currHeader = nextHeader(currHeader); //iterates to next header
   }
   
   printf("mymalloc, Memory out of bounds error: not enough memory to accommodate request: %s, line %d, malloc(%d) \n", file, line, og_size); //error statement
   return NULL; //returns null if can't find availablie size for requested memory size
}

void myfree(void *ptr, char *file, int line) {
   memData* currHeader = (memData*)memory; //intitalize currHeader to start of memory region
   ptr = (memData*)(ptr - HEAD_SIZE);
   
   //checks if the ptr even exists in the memory, if not returns address error not obtained from malloc
   if ((memData*)ptr < (memData*)memory || (memData*)ptr >= (memData*)(memory + MEMLENGTH)) {
      printf("myfree, Address error: not obtained from mymalloc(): in %s, line %d, free(%p) \n", file, line, ptr);
      return;
   } 

   //loops through memory to find header equivalent to ptr (memory chunk that wants to be freed)
  while (currHeader < (memData*)(memory + MEMLENGTH)) {
      if (currHeader == ptr && isAllocated(currHeader)) {
         currHeader->allocated = 0; //frees memory chunk
         combineSegments(); //coalesces small chunks into big ones if any exists
         return;
      }
      else {
         if (currHeader == ptr){
            printf("myfree, Double Free error: calling free twice on same ptr: %s, line %d, free(%p) \n", file, line, ptr);
            return;
         }
         else{
            currHeader = nextHeader(currHeader); //iterates to next header
         }
      }
   }

   printf("myfree, Address error: not at start of chunk: %s, line %d, free(%p) \n", file, line, ptr);
   return;
}