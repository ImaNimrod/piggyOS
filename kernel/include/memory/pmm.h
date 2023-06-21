#ifndef _KERNEL_PMM_H
#define _KERNEL_PMM_H

#include <display.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <system.h>

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BUCKET 8

// macros for setting/clearing bits in a bitmap
#define SETBIT(i) bitmap[i / BLOCKS_PER_BUCKET] = bitmap[i / BLOCKS_PER_BUCKET] | (1 << (i % BLOCKS_PER_BUCKET))
#define CLEARBIT(i) bitmap[i / BLOCKS_PER_BUCKET] = bitmap[i / BLOCKS_PER_BUCKET] & (~(1 << (i % BLOCKS_PER_BUCKET)))
#define ISSET(i) ((bitmap[i / BLOCKS_PER_BUCKET] >> (i % BLOCKS_PER_BUCKET)) & 0x1)
#define GET_BUCKET32(i) (*((uint32_t*) &bitmap[i / 32]))
#define ALIGN(addr) (((uint32_t) (addr) & 0xFFFFF000) + BLOCK_SIZE)

/* function declarations */
void pmm_init(size_t mem_size);
uint32_t allocate_block();
void free_block(size_t blk_num);
uint32_t first_free_block();

#endif
