#include "memory.h"

void *mm_malloc(Memory mem, int32_t amount) {
    Memory new = (Memory) malloc(sizeof(struct mm_node));

    while (mem->next) mem = mem->next;

    new->addr = malloc(amount);
    new->next = NULL;
    new->free = 0;
    new->size = amount;

    if (new->addr == NULL) {
        free(new);
        return NULL;
    }

    mem->next = new;
}

void *mm_fetch(Memory mem, int32_t amount) {
    Memory tmp;

    for (tmp = mem; tmp != NULL; tmp = tmp->next) {
        if (tmp->free != MM_FREE) continue;

        if (tmp->size < amount) continue;

        tmp->free = MM_USED;
        return tmp->addr;
    }

    return mm_malloc(mem, amount);
}

void mm_free(Memory mem, void *addr) {
    for (; mem != NULL; mem = mem->next) {
        if (mem->addr == addr) {
            mem->free = MM_FREE;
            return;
        }
    }
}

void mm_release(Memory mem) {
    Memory tmp;
    while (mem != NULL) {
        tmp = mem->next;

        free(mem->addr);
        free(mem);

        mem = tmp;
    }
}