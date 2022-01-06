/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

#include "heapsys.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};



#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

static struct HeapSystem *heap;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{

    // check pagesize settings
    if (PAGE_SIZE < ALIGN(sizeof(struct HeapBigBlock)) + 8 * BLOCK_LIST_MAXIDX
        || ALIGN(sizeof(struct HeapBigBlock)) == ALIGN(sizeof(struct HeapBlockHeader))
    ) {
        printf("some things are incompatible...\n");
        exit(1);
    }

    // allocate heap object
    heap = mem_sbrk(ALIGN(sizeof(struct HeapSystem)));
    if (heap == (void *)-1) {
        return -1;
    }

    // printf("heap object allocated\n");

    // init
    heapSystemInit(heap);

    // printf("heap object init\n");

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    /*int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }*/

    // printf("heap state big block head %p\n", heap->bighead);

    if (size == 0) {
        return NULL;
    }
    // printf("a size %u\n", size);
    void *p;
    if (allocateHeapSystem(heap, size, &p) < 0) {
        return (void *)-1;
    }
        // printf("after malloc heap state big block head 0x%p\n", heap->bighead);
    return p;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    // printf("f address %p\n", ptr);
    deallocateHeapSystem(heap, ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














