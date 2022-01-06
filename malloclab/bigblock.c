#include "heapsys.h"
#include "memlib.h"

/*
*   The functions in this file cares about the free list management of big blocks.
*   They implement the giving and returning of the big blocks.
*   They neither know, nor do they care about the content of the blocks when the blocks are not registered on the free list.
*   Nevertheless, when the blocks are returned, the caller of the return function
*   must know how much was given starting at the address that it is returning.
*   Therefore, some book-keeping must be maintained, but not by this file.
*/

void *bigBlockWithHeaderPayload(struct HeapBigBlock *bigblock) {
    return (void *)bigblock + ALIGN(sizeof(struct HeapBigBlock));
}

int allocateOneBigBlock(struct HeapBigBlock **retp) {
    void * p = mem_sbrk(PAGE_SIZE);
    if (p == (void *)-1) {
        // sbrk failed
        printf("sbrk failed... cannot allocate one big block...\n");
        return -1;
    }
    struct HeapBigBlock *block = p;
    block->refCount = 0;
    if (retp) {
        *retp = block;
    }
    return 0;
}

int fetchABigBlock(struct HeapBigBlock **retp, struct HeapBigBlock **head) {
    // printf("fetching a big block... ");
    if (*head) {
        // printf("available free big block 0x%p!\n", *head);
        if (retp) {
            *retp = *head;
        }
        *head = (**head).next;
        return 0;
    }
    // printf("no free big block.. allocating...\n");
    return allocateOneBigBlock(retp);
}

int fetchNBigBlocks(struct HeapBigBlock **retp, unsigned int n, struct HeapBigBlock **head) {
    // printf("fetching %u big blocks\n", n);
    if (n == 1) {
        return fetchABigBlock(retp, head);
    }
    // return the first one
    if (allocateOneBigBlock(retp) < 0) {
        printf("cannot allocate continuous big blocks...\n");
        return -1;
    }
    for (int i = 0; i < n - 1; ++i) {
        if (allocateOneBigBlock(NULL) < -1) {
            printf("cannot allocate continuous big blocks...\n");
            return -1;
        }
    }
    // printf("%p fetched\n", *retp);
    return 0;
}

void returnABigBlock(struct HeapBigBlock *block, struct HeapBigBlock **head) {
    block->next = *head;
    *head = block;
}
