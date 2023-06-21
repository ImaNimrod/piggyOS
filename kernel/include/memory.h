#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <cpu/isr.h>
#include <display.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define MAX_PAGE_ALIGNED_ALLOCS 32

typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

/* function declarations */
void mmu_init(uint32_t* kernel_end);
void mmu_print_state(void);

void* kmalloc(size_t size);
void kfree(void *mem);

void paging_init(void);

#endif 
