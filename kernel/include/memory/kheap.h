#ifndef _KERNEL_KHEAP_H
#define _KERNEL_KHEAP_H

#include <display.h>
#include <memory/vmm.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <system.h>

#define KHEAP_START         (uint8_t*) LOAD_MEMORY_ADDRESS + 0x00400000 
#define KHEAP_INITIAL_SIZE  8 * M
#define KHEAP_MAX_ADDRESS   (uint8_t*) LOAD_MEMORY_ADDRESS + 0x0FFFFFFF
#define KHEAP_MIN_SIZE      4 * M

#define OVERHEAD (sizeof(struct Block) + sizeof(size_t))

struct Block {
    size_t size;
    struct Block* prev;
    struct Block* next;
};

void kheap_init(void* start, void* end, void* max);

uint32_t kmalloc_int(uint32_t sz, int align, uint32_t *phys);
void* kmalloc_cont(uint32_t sz, int align, uint32_t *phys);
uint32_t kmalloc_a(uint32_t sz);
uint32_t kmalloc_p(uint32_t sz, uint32_t *phys);
uint32_t kmalloc_ap(uint32_t sz, uint32_t *phys);
void* kmalloc(uint32_t sz);
void kfree(void *p);
void* krealloc(void * ptr, uint32_t size);

int doesItFit(struct Block * n, uint32_t size);
void setFree(uint32_t *size, int x);
void removeNodeFromFreelist(struct Block * x);
void addNodeToFreelist(struct Block * x);
int isBetter(struct Block * node1, struct Block * node2);

struct Block* bestfit(uint32_t size);
struct Block* getPrevBlock(struct Block* n);
struct Block* getNextBlock(struct Block* n);

uint32_t getRealSize(uint32_t size);
uint32_t getSbrkSize(uint32_t size);
int isFree(struct Block* n);
int isEnd(struct Block* n);


void *malloc(size_t size);
void free(void * ptr);
void *kcalloc(uint32_t num, uint32_t size);
void *realloc(void *ptr, uint32_t size);

#endif
