#ifndef _KERNEL_PMM_H
#define _KERNEL_PMM_H

#include <display.h>
#include <multiboot2.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <system.h>

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BUCKET 8

// macros for setting/clearing bits in a phys_mem_bitmap
#define PMM_BITMAP_SET(i) phys_mem_bitmap[i / BLOCKS_PER_BUCKET] = phys_mem_bitmap[i / BLOCKS_PER_BUCKET] | (1 << (i % BLOCKS_PER_BUCKET))
#define PMM_BITMAP_CLEAR(i) phys_mem_bitmap[i / BLOCKS_PER_BUCKET] = phys_mem_bitmap[i / BLOCKS_PER_BUCKET] & (~(1 << (i % BLOCKS_PER_BUCKET)))
#define PMM_BITMAP_ISSET(i) ((phys_mem_bitmap[i / BLOCKS_PER_BUCKET] >> (i % BLOCKS_PER_BUCKET)) & 0x1)
#define ALIGN(addr) (((uint32_t) (addr) & 0xFFFFF000) + BLOCK_SIZE)

/* function declarations */
void pmm_init(struct mboot_tag_basic_meminfo* meminfo);
uint32_t pmm_alloc_block(void);
void pmm_free_block(size_t blk_num);

#endif
