#ifndef CACHE_SYS_H
#define CACHE_SYS_H


// 32b total address

struct CacheBlock {
    int tagVal;
    int present;
    long timestamp;   // for lru
    int dirty;       // for lru
};

struct CacheSet {
    struct CacheBlock *blockTable;
};

struct CacheSystem {
    int setIndexBitCount;
    int associativityCount;
    int blockBitCount;
    int tagBitCount;

    struct CacheSet *setTable;

    long timestamp;  // for lru

    // stats
    int hits, misses, evicts;
};

void accessCache(struct CacheSystem *cache, unsigned int address, int isWrite);
void cacheInit(struct CacheSystem *cache, int s, int E, int b);
void cacheDestroy(struct CacheSystem *cache);

#endif