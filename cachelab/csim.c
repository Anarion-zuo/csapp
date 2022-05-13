#include "cachelab.h"
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "cache_sys.h"

struct Config {
    int s;
    int E;
    int b;
    const char *tracefile;
    int h;
    int v;
};

void initConfig(struct Config *config) {
    config->s = -1;
    config->E = -1;
    config->b = -1;
    config->tracefile = NULL;
    config->h = 0;
    config->v = 0;
}

int parseArgs(struct Config *config, int argc, char *argv[]) {
    for (int i = 1; i < argc;) {
        char *name = argv[i];
        int nameLen = strlen(name);
        if (nameLen <= 0 || nameLen > 2) {
            return -1;
        }
        if (name[0] != '-') {
            return -1;
        }
        int val;
        switch (name[1])
        {
        case 's':
        case 'E':
        case 'b':
            if (i + 1 > argc) {
                return -1;
            }
            char *content = argv[i + 1];
            int contentLen = strlen(content);
            if (contentLen <= 0) {
                return -1;
            }
            val = strtol(content, NULL, 10);
            if (errno) {
                return -1;
            }
            i += 2;
            break;
        case 't':
            if (i + 1 > argc) {
                return -1;
            }
            config->tracefile = argv[i + 1];
            i += 2;
            break;
        case 'h':
            config->h = 1;
            ++i;
            break;
        case 'v':
            config->v = 1;
            ++i;
            break;
        default:
            return -1;
        }
        switch (name[1])
        {
        case 's':
            config->s = val;
            break;
        case 'E':
            config->E = val;
            break;
        case 'b':
            config->b = val;
            break;
        }
    }
    return 0;
}

int isConfigComplete(struct Config *config) {
    if (config->b == -1) {
        return 1;
    }
    if (config->E == -1) {
        return 1;
    }
    if (config->s == -1) {
        return 1;
    }
    if (config->tracefile == NULL) {
        return 1;
    }
    return 0;
}

int parseLine(const char *str, struct CacheSystem *cache) {
    int len;
    for (int i = 0; true; ++i) {
        if (str[i] == '\n' || str[i] == 0) {
            len = i;
            break;
        }
    }
    if (len == 0) {
        return 0;
    }
    char op;
    unsigned int address;
    int size;
    if (str[0] == 'I') {
        // is I
        // printf("found I\n");
        return len;
    } else {
        sscanf(str, " %c %x,%d\n", &op, &address, &size);
    }
    // printf("line parsed, issueing command...\n");
    // printf("%c 0x%x %d :: %d\n", op, address, size, len);
    // issue command
    switch (op)
    {
    case 'L':
        // load
        accessCache(cache, address, 0);
        break;
    case 'S':
        // store
        accessCache(cache, address, 0);
        break;
    case 'M':
        // load then store
        accessCache(cache, address, 0);
        accessCache(cache, address, 1);
        break;
    case 'I':
        // ignores instruction
        break;
    default:
        perror("invalid op code\n");
        exit(1);
    }
    return len;
}

void printCacheStats(struct CacheSystem *cache) {
    printSummary(cache->hits, cache->misses, cache->evicts);
}

int main(int argc, char *argv[])
{
    // load configuration from terminal arguments

    if (argc < 1) {
        return 1;
    }
    struct Config config;
    initConfig(&config);
    if (parseArgs(&config, argc, argv) == -1) {
        perror("args format incorrect...\n");
        printSummary(0, 0, 0);
        return 0;
    }
    // printf("set index bits %d, associativity %d, block bits %d, tracefile %s\n", config.s, config.E, config.b, config.tracefile);
    if (config.h == 1) {
        printf("help... ignoring other args...\n");
        printSummary(0, 0, 0);
        return 0;
    }
    if (isConfigComplete(&config)) {
        perror("args incomplete...\n");
        printSummary(0, 0, 0);
        return 0;
    }

    // open file
    // prepare dir
    char dir[80];
    // sprintf(dir, "traces/%s", config.tracefile);
    sprintf(dir, "%s", config.tracefile);
    FILE *f = fopen(dir, "r");
    if (f == NULL) {
        fprintf(stderr, "cannot open trace file %s...\n", dir);
        return 0;
    }
    // printf("trace file opened\n");

    // read file
    fseek(f, 0, SEEK_END);
    int filesize = ftell(f);
    rewind(f);
    char *filebuffer = malloc(filesize + 1);
    int readlen = fread(filebuffer, 1, filesize, f);
    if (readlen != filesize) {
        perror("failed to read trace file...\n");
        return 0;
    }
    filebuffer[filesize] = '\0';
    // printf("trace file loaded\n");

    // init cache system
    struct CacheSystem cache;
    cacheInit(&cache, config.s, config.E, config.b);
    // printf("cache struct initialized\n");

    // parse
    const char *str = filebuffer;
    while (true) {
        int len = parseLine(str, &cache);
        if (len == 0) {
            break;
        }
        str += len + 1;
    }

    printCacheStats(&cache);

    // close everything
    cacheDestroy(&cache);
    free(filebuffer);
    fclose(f);
    return 0;
}
