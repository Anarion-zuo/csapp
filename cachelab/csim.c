#include "cachelab.h"
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

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

int main(int argc, char *argv[])
{
    // load configuration from terminal arguments

    if (argc < 1) {
        return 1;
    }
    struct Config config;
    initConfig(&config);
    if (parseArgs(&config, argc, argv) == -1) {
        // perror("args format incorrect...\n");
        printSummary(0, 0, 0);
        return 0;
    }
    printf("set index bits %d, associativity %d, block bits %d, tracefile %s\n", config.s, config.E, config.b, config.tracefile);
    if (config.h == 1) {
        // printf("help... ignoring other args...\n");
        printSummary(0, 0, 0);
        return 0;
    }
    if (isConfigComplete(&config)) {
        // perror("args incomplete...\n");
        printSummary(0, 0, 0);
        return 0;
    }

    // load

    printSummary(0, 0, 0);
    return 0;
}
