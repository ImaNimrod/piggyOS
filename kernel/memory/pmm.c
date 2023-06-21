#include <memory/pmm.h>

uint32_t total_blocks;
uint8_t* phys_mem_bitmap = (uint8_t*) (&kernel_end);
uint32_t phys_mem_bitmap_size;

static uint32_t first_free_block(void) {
    for(size_t i = 0; i < total_blocks; i++) {
        if(!ISSET(i))
            return i;
    }
    klog(LOG_WARN, "%s: running out of free blocks\n", __func__);
    return (uint32_t) -1;
}

uint32_t pmm_alloc_block(void) {
    uint32_t free_block = first_free_block();
    SETBIT(free_block);
    return free_block;
}

void pmm_free_block(size_t blk_num) {
    CLEARBIT(blk_num);
}

void pmm_init(struct mboot_tag_basic_meminfo* meminfo) {
    uint32_t phys_mem_size = (meminfo->mem_lower + meminfo->mem_upper) * K;
    total_blocks = phys_mem_size / BLOCK_SIZE; 
    phys_mem_bitmap_size = total_blocks / BLOCKS_PER_BUCKET;
    if(phys_mem_bitmap_size* BLOCKS_PER_BUCKET < total_blocks)
        phys_mem_bitmap_size++;

    memset(phys_mem_bitmap, 0, phys_mem_bitmap_size);

    for(size_t i = 0; i < phys_mem_bitmap_size; i++) {
        if(phys_mem_bitmap[i] != 0) {
            klog(LOG_WARN, "%s: memory phys_mem_bitmap is not all empty, fix it!\n", __func__);
        }
    }
    klog(LOG_OK, "Physical Memory Manager initialized\n");
}
