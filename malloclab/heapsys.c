#include "heapsys.h"
#include "memlib.h"
#include <stdio.h>

int towToTheN(unsigned char n) {
    return 1u << n;
}

int sizeToBlockIndex(size_t size) {
    int index = size / 8, more = size % 8;
    if (more == 0) {
        --index;
    }
    return index;
}

/**
 * @brief initialized the given structure
 * 
 * @param heap 
 */
void heapSystemInit(struct HeapSystem *heap) {
    // empty small block list
    for (int i = 0; i < BLOCK_LIST_MAXIDX; ++i) {
        heap->blockList[i] = NULL;
    }
    // empty big block list
    heap->bighead = NULL;
}

int allocateASmallBlock(struct HeapBlock **list, int index, struct HeapBigBlock **head, void **retp);
int fetchNBigBlocks(struct HeapBigBlock **retp, unsigned int n, struct HeapBigBlock **head);

int allocateHeapSystem(struct HeapSystem *heap, size_t size, void **retp) {
    // printf("heap state big block head %p\n", heap->bighead);
    // printf("a %u \n", size);
    int index = sizeToBlockIndex(size);
    if (index < 0) {
        // allocating 0 size
        if (retp) {
            *retp = NULL;
        }
        printf("too little\n");
        return 0;
    }
    if (index >= BLOCK_LIST_MAXIDX) {
        size += ALIGN(sizeof(struct HeapBlockHeader));
        unsigned int count = size / PAGE_SIZE, more = size % PAGE_SIZE;
        if (more) {
            ++count;
        }
        // printf("big block count %u\n", count);
        struct HeapBigBlock *bigblock;
        if (fetchNBigBlocks(&bigblock, count, &heap->bighead) < 0) {
            printf("cannot fetch multiple big blocks...\n");
            return -1;
        }
        // printf("after fetchN heap state big block head 0x%p\n", heap->bighead);
        struct HeapBlockHeader *header = (struct HeapBlockHeader *)bigblock;
        header->size = size;
        if (retp) {
            *retp = (void *)header + ALIGN(sizeof(struct HeapBlockHeader));
        }
        return 0;
    }
    // printf("small block index %u\n", index);
    return allocateASmallBlock(heap->blockList, index, &heap->bighead, retp);
}

void returnABigBlock(struct HeapBigBlock *block, struct HeapBigBlock **head);
void returnASmallBlock(struct HeapBlock **list, struct HeapBigBlock **head, struct HeapBlock *block);
int isABigBlock(void *p);

void deallocateHeapSystem(struct HeapSystem *heap, void *p) {
    // size_t size = header->size;
    if (isABigBlock(p)) {
        struct HeapBlockHeader *header = p - ALIGN(sizeof(struct HeapBlockHeader));
        unsigned int size = header->size;
        // deallocating big blocks
        unsigned int count = size / PAGE_SIZE, more = size % PAGE_SIZE;
        if (more) {
            ++count;
        }
        // printf("returning %u big blocks\n", count);
        for (int i = 0; i < count; ++i) {
            returnABigBlock((void *)header + i * PAGE_SIZE, &heap->bighead);
        }
        return;
    }
    // deallocating a small block
    returnASmallBlock(heap->blockList, &heap->bighead, p);
}
