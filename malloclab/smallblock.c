#include "heapsys.h"

struct HeapBigBlock *smallToBig(struct HeapBlock *block) {
    // compute distance to the first allocated big block
    size_t offset = (void *)block - (mem_heap_lo() + ALIGN(sizeof(struct HeapSystem)));
    size_t index = offset / PAGE_SIZE;
    return (mem_heap_lo() + ALIGN(sizeof(struct HeapSystem))) + index * PAGE_SIZE;
}

void *blockHeaderPayload(struct HeapBlockHeader *header) {
    return (void *)header + ALIGN(sizeof(struct HeapBlockHeader));
}

void bigBlockStepInit(struct HeapBigBlock *bigblock, unsigned int step, unsigned int offset) {
    bigblock->step = step;
    void *p = (void *)bigblock + offset;
    // begin address
    bigblock->payload = p;
    // printf("step init begin 0x%p step %u\n", p, step);
    // make lined list
    for (; p < (void *)bigblock + offset + PAGE_SIZE - step; p += step) {
        // printf("0x%p\n", p);
        struct HeapBlock *node = p;
        node->next = p + step;
    }
    ((struct HeapBlock *)p)->next = NULL;
}

int fetchABigBlock(struct HeapBigBlock **retp, struct HeapBigBlock **head);

int allocateASmallBlock(struct HeapBlock **list, int index, struct HeapBigBlock **head, void **retp) {
    struct HeapBlock **listnode = &list[index], *retnode;
    if (!*listnode) {
        // must allocate a big block
        struct HeapBigBlock *bigblock;
        // fetch a big block
        if (fetchABigBlock(&bigblock, head) < 0) {
            return -1;
        }
        // printf("big block 0x%p\n", bigblock);
        bigBlockStepInit(bigblock, 8 * (index + 1) + ALIGN(sizeof(struct HeapBlockHeader)), ALIGN(sizeof(struct HeapBigBlock)));
        *listnode = bigblock->payload;
    }
    // allocate from a fetched big block
    retnode = *listnode;
    // pop linked list
    *listnode = (**listnode).next;
    // ref count
    smallToBig(retnode)->refCount++;
        // printf("heap state big block head %p\n", *head);

    // init small block header
    struct HeapBlockHeader *header = (struct HeapBlockHeader *)retnode;
    header->size = 8 * (index + 1);
    if (retp) {
        *retp = blockHeaderPayload(header);
    }
    return 0;
}

void returnABigBlock(struct HeapBigBlock *block, struct HeapBigBlock **head);

void returnASmallBlock(struct HeapBlock **list, int index, struct HeapBigBlock **head, struct HeapBlock *block) {
    struct HeapBlock **listnode = &list[index];
    // struct HeapBlock *block = p - ALIGN(sizeof(struct HeapBlockHeader));
    block->next = *listnode;
    *listnode = block;

    // ref count
    struct HeapBigBlock *bigblock = smallToBig(block);
    bigblock->refCount--;
    if (bigblock->refCount == 0) {
        // can return big block
        returnABigBlock(bigblock, head);
    }
}
