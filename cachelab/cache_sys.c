#include "cache_sys.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

int cacheSetCount(struct CacheSystem *cache) {
    return (1 << cache->setIndexBitCount);
}

int cacheAddressToSetIndex(struct CacheSystem *cache, unsigned int address) {
    return (address << cache->tagBitCount) >> (32 - cache->setIndexBitCount);
}

int cacheAddressToTag(struct CacheSystem *cache, unsigned int address) {
    return address >> (32 - cache->tagBitCount);
}

void cacheInit(struct CacheSystem *cache, int s, int E, int b) {
    cache->associativityCount = E;
    cache->setIndexBitCount = s;
    cache->blockBitCount = b;

    cache->tagBitCount = 32 - (cache->setIndexBitCount + cache->blockBitCount);

    // construct set table
    cache->setTable = malloc(sizeof(struct CacheSet) * cacheSetCount(cache));
    if (cache->setTable == NULL) {
        perror("malloc failed\n");
        exit(1);
    }
    for (int i = 0; i < cacheSetCount(cache); ++i) {
        cache->setTable[i].blockTable = malloc(sizeof(struct CacheBlock) * cache->associativityCount);
        for (int j = 0; j < cache->associativityCount; ++j) {
            struct CacheBlock *block = &cache->setTable[i].blockTable[j];
            block->tagVal = 0;
            block->present = 0;
            block->timestamp = 0;  // lru
            block->dirty = 0;      // lru
        }
    }

    // setup lru timestamp
    cache->timestamp = 0;

    // stats
    cache->hits = 0;
    cache->misses = 0;
    cache->evicts = 0;
}

void cacheDestroy(struct CacheSystem *cache) {
    for (int i = 0; i < cacheSetCount(cache); ++i) {
        free(cache->setTable[i].blockTable);
    }
    free(cache->setTable);
}

void bringInABlock(struct CacheSystem *cache, struct CacheBlock *block, int tag) {
    block->tagVal = tag;
    block->present = 1;
    block->dirty = 0;
}

int compareBlockTimeStamp(const void *s1, const void *s2) {
    struct CacheBlock *b1 = (struct CacheBlock *)s1;
    struct CacheBlock *b2 = (struct CacheBlock *)s2;
    if (b1->timestamp < b2->timestamp) {
        return -1;
    } /*else if (b1->timestamp == b2->timestamp) {
        if ()
        return 0;
    } else {
        return 1;
    }*/
    else if (b1->timestamp > b2->timestamp) {
        return 1;
    }
    if (b1->dirty && !b2->dirty) {
        return 1;
    } else if (!b1->dirty && b2->dirty) {
        return -1;
    }
    return 0;
}

int findABlock(struct CacheSystem *cache, struct CacheSet *set, int tag) {
    // find tag match
    // printf("finding a block\n");
    for (int i = 0; i < cache->associativityCount; ++i) {
        struct CacheBlock *block = &set->blockTable[i];
        // printf("looking at block %d\n", i);
        if (block->tagVal == tag) {
            if (!block->present) {
                // miss!
                printf("cache missed!\n");
                bringInABlock(cache, block, tag);
                cache->misses++;
                return i;
            } else {
                // hit!
                printf("cache hit!\n");
                cache->hits++;
                return i;
            }
        }
    }
    // no tag matches
    // find free
    for (int i = 0; i < cache->associativityCount; ++i) {
        struct CacheBlock *block = &set->blockTable[i];
        if (!block->present) {
            // empty slot!
            printf("cache missed!\n");
            bringInABlock(cache, block, tag);
            cache->misses++;
            return i;
        }
    }
    // no empty slot
    // must evict
    // sort from the smallest to the greatest
    printf("evicting\n");
    qsort(set->blockTable, cache->associativityCount, sizeof(struct CacheBlock), compareBlockTimeStamp);
    
    // find lru
    // long minStamp = INT_MAX;
    // int lruIndex = -1;
    // for (int i = 0; i < cache->associativityCount; ++i) {
    //     struct CacheBlock *block = &set->blockTable[i];
    //     if (block->timestamp < minStamp) {
    //         lruIndex = i;
    //         minStamp = block->timestamp;
    //     }
    // }

    // struct CacheBlock *block = &set->blockTable[lruIndex];
    struct CacheBlock *block = &set->blockTable[0];
    bringInABlock(cache, block, tag);
    cache->misses++;
    cache->evicts++;
    return 0;
}

void accessCache(struct CacheSystem *cache, unsigned int address, int isWrite) {
    cache->timestamp++;

    int tag = cacheAddressToTag(cache, address);
    int setIndex = cacheAddressToSetIndex(cache, address);
    printf("address 0x%x tag 0x%x index 0x%x ", address, tag, setIndex);
    // fetch the set
    struct CacheSet *set = &cache->setTable[setIndex];
    // printf("set fetched by index\n");
    // give me a block
    int blockn = findABlock(cache, set, tag);
    set->blockTable[blockn].timestamp = cache->timestamp;
    set->blockTable[blockn].dirty |= isWrite;
}

