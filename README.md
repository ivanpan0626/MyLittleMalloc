My Little Malloc(), CS214 SPR24

Project by: Ivan Pan(ip278), Aysenur Kochan(ak1965)

Implementation of Malloc(), Free(): Design
**mymalloc.c**; 
    -memData, an 8 byte size data struct to handle metadata.
            -unsigned int dsize for total size of header + actual allocated memory size.
            -unsigned int allocated for checking whether the chunk is allocated(1) or freed(0).
    -Includes 3 private helper functions
        -isAllocated(memData* header), checks memData for ->allocated value. Returns 0 for unallocated and 1 for allocated.
        -nextHeader(memData* header), linked list implementation, moves current ptr header to next header in line.
        -combineSegments(), coalesce function, called by myfree() function whether a chunk is freed. It sets a ptr at the first header of the memory, and then checks if the next header exists and is free. If true, it will combine the two chunks size with the current header. If not, it will skip the next header and go to the next next header to repeat the process ONLY AFTER checking if next next header even exists in memory region.
    -mymalloc(), 
        -mymalloc() function takes three parameters; size, file, and line. 
            -The size parameter is used to reserve the requested appropiate memory space for the client.
            -The file and line parameter is used to report where errors occured in the client code.
        -first checks if requested size is 0 or exceeds total memory size.
            -malloc(0) edge case, we chose not to allocate space and return as an error.
        -intializes memory region for memory allocations by checking if header->size == 0, 
            -then sets header->size to total memory if uninitialized.
        -While loop that iterates through all the headers within the memory region.
            -checks simultaneously if the current chunk is large enough and unallocated for the requested size, it proceeds to check if the chunk is large enough to be split to save rest of memory for further uses.
                -creates a new header at the current address but offsetted by the requested size + HEAD_SIZE(header size) to handle the newly made chunk.
            -if no headers that fit the requested size or are all allocated, it returns the memory out of bounds error.
    -myfree(),
        -myfree() function takes three parameters; ptr, file, and line. 
            -The ptr parameter is used to find the header that it matches with and proceeds to free it afterwards.
            -The file and line parameter is used to report where errors occured in the client code.
        -always first checks if the ptr is even an ptr in the address space of the memory region
            -if yes proceeds to check for a match betwee ptr and headers
            -if not returns an error that is not within memory.
        -While loop to iterate through all headers in the memory region
            -simultaneously checks if header is a match with ptr and if it is allocated.
                -if both are true it proceeds to free the chunk and perform combineSegments() function as mentioned above.
                -if false it checks if the ptr is pointing an already freed header. If true, it will return double free error
        -if nothing is found in the while loop, it will return not at start of chunk error, since ptr is in the memory region but it matches none of the headers.

**memgrind.c**; 
    --PERFORMANCE TESTINGS--
	1.  oneByte120T(): malloc() and immediately free() a 1-byte object, 120 times.

	2.	arrPtr1Byte120T(): Use malloc() to get 120 1-byte objects, storing the pointers in an array, then use free() to deallocate the chunks.

	3.	randomMalloc1Byte120T(): Creates an array of 120 pointers to repeatedly make a random choice between allocating a 1-byte object while adding the pointer to the array and deallocating a previously allocated object (if any), stops once 120 allocations are done.

	4.	reverseFree20Byte120T(): Uses malloc() to get 120 20-bytes objects, storing the pointer in an array, then uses free() in reverse order to deallocate to stress coalesce. Then malloc() a 4080-bytes object to check if coalesce performed.

	5.	randomByteAllocation120T(): Similar to memgrind test 3 with the addition of random value of bytes being malloc'ed.

    --Average run times taken for each test-- 
1-  AVG TIME FOR MALLOC/FREE 1 BYTE 120 TIMES: *0.000006 seconds*
2-  AVG TIME FOR MALLOC/FREE ARRAY OF 120 PTR OF 1 BYTE: *0.000096 seconds*
3-  AVG TIME FOR RANDOM MALLOC/FREE ARRAY OF 120 PTR OF 1 BYTE: *0.000035 seconds*
4-  AVG TIME FOR REVERSE FREEING MEMORY OF 10 BYTES AND ALLOCATING WHOLE CHUNK TO SEE COALESCE: *0.000124 seconds*
5-  AVG TIME FOR RANDOM MALLOC/FREE ARRAY OF 120 PTR OF RANDOM BYTES: *0.000026 seconds*

**testCases.c**
    --CORRECTNESS TESTING AND DETECTABLES ERRORS--
    1.  mallocReserveUnallocMem(): Simply checks if the malloc() reserves unallocated memory for use.

    2.  mallocPtrOverlap: checks if the header ptr created does not overlapped with any other allocated objects.

    3.  mallocDistinctBytes: Serveral objects with distinct bytes are filled into the memory, checks if the distinct bytes get overwritten, if not, its good. Same idea as memtest.c

    4.  free_coalesced: malloc() varying sizes of memory and then proceeds to free them in a random order to check if myfree() correctly works, while also checking if coalescing is performing by mallocing the whole region to see if all the chunks merged back to one big chunk.
    
    5.  endOfMemory():  malloc() nearly the entire size of memory leaving only 8 bytes to make sure that malloc will not perform when there is only 8 bytes left. MIN byte need is size of metadata + MIN requested size, which is 8+8 for us. So minimum 16 bytes is needed. It will send out an error, which indicates the malloc() performed as expected.

    6.  corrAlignment(): malloc() an object of size 3980, which by 8 byte alignment will be a size of 3984. Then proceeds to malloc() another object of 100-byte which should fail, as 3984 + 8(head_size)+ 100(new malloc)+ 8(new head_size) = 4100 > 4096 (total size). Therefore, error message should be thrown and the next malloc() of 88-byte should work. If this is all passed then the 8 byte alignment worked as intended.

    7.  detectableErrors: Multiple test cases to check for errors that is normally not checked by normal malloc() but covered in mymalloc(). 
        -Checks for free thats not in memory region
        -Checks for freeing ptr not at start of chunk
        -Checks for double free error
        -Checks for malloc(0), memory too small
        -Checks for malloc(4097), exceeds total memory size

**memtest.c**
    --PROVIDED DISTINCT BYTES TEST--
0 distinct bytes error found

__HOW TO COMPILE__
$ make
executables: ./memgrind ./testCases ./memtest