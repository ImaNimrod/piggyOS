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
void mmu_init(uintptr_t* kernel_end);
void mmu_print_state(void);

char* kmalloc(size_t size);
void kfree(void *mem);

char* kpmalloc(void);
void kpfree(void *mem);

void paging_init(void);

#endif 
