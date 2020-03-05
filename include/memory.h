#include <stdlib.h>
#include <stdint.h>

#define MM_USED 0x0
#define MM_FREE 0x1

#define mm_calloc(mem, type, amount) mm_malloc(mem, sizeof(type) * amount)

typedef struct mm_node {
    void *addr;
    struct mm_node *next;
    uint8_t free;
    int32_t size;
} *Memory;

void *mm_malloc(Memory mem, int32_t amount);

void *mm_fetch(Memory mem, int32_t amount);

void  mm_free(Memory mem, void *addr);

void  mm_release(Memory mem);