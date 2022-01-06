#ifndef HEAPSYS_H
#define HEAPSYS_H

#include <stdlib.h>
#include "memlib.h"

#define BLOCK_LIST_MAXIDX 32
#define PAGE_SIZE 8192

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

/**
 * @brief 
 * When a small block is not allocated, it should contain this struct as its header.
 * This header can be overwritten after the block is allocated, so it takes no extra space.
 * 
 */
// 首地址 arr 或 &arr[0]
// arr[i]是 *(arr + i)
// 地址是 arr + i
// 要得到地址就是 &arr[i] 也就是 &*(arr + i) 也就是 arr + i
struct HeapBlock {
    struct HeapBlock *next;
    // struct HeapBigBlock *bigblock;        // who does this one belong to
};

/**
 * @brief
 * When a block is allocated, big or small, it should contain this struct as its header.
 * This header cannot be overwritten by payload, so this struct takes extra space.
 * 
 */
struct HeapBlockHeader {
    size_t size;
};

/**
 * @brief 
 * When a big block is not allocated, it should contain this struct as its header.
 * After the big block is allocated, there are 2 different circumstances.
 * 
 * If the big block is allocated for the purpose of holding small blocks, this header cannot be overwritten.
 * If the big block is allocated for the purpose of resolving large allocations, this header can be overwritten.
 * 
 */
struct HeapBigBlock {
    unsigned int step;         // which size does this big block serve
    struct HeapBigBlock *next;    // pointer to the next block
    unsigned int refCount;     // how many small blocks are allocated on this big block
    void *payload;             // where the actual payload begins
};
/**
 * @brief 
 * blockList is the free list headers of small blocks.
 * bighead is the free list header of big blocks.
 * 
 * When allocating small blocks, the allocator looks at its small block free lists.
 * It finds one free small block if available.
 * If there is no small block on the free list, fetch a big block from the big block list to have some.
 * If there is no big block on the free list, call brk to have a new one.
 * 
 * When allocating big blocks, the allocator looks at its big block free list.
 * It computes how much blocks it requires.
 * 
 * If it requires more than 1 block, it allocates some new ones, instead of fetching from the linked list.
 * This is because the big blocks on the free list may not be continuous to one another.
 * If it requires only 1 block, it can well fetch one from the free list.
 * 
 */
struct HeapSystem {
    struct HeapBlock *blockList[BLOCK_LIST_MAXIDX];
    struct HeapBigBlock *bighead;
};


void heapSystemInit(struct HeapSystem *heap);
int allocateHeapSystem(struct HeapSystem *heap, size_t size, void **retp);
void deallocateHeapSystem(struct HeapSystem *heap, void *p);


#endif /* HEAPSYS_H*/