#ifndef _KERNEL_PMM_H
#define _KERNEL_PMM_H

#include <display.h>
#include <multiboot2.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <system.h>

#define PMM_BLOCK_SIZE 4096

/* function declarations */
void pmm_init(struct mboot2_begin* mb2);
uint32_t pmm_alloc_block(void);
void pmm_free_block(uint32_t blk_num);

#endif
