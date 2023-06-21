#include <memory/pmm.h>

uint8_t* bitmap = (uint8_t*) (&kernel_end);
uint8_t* mem_start;
uint32_t total_blocks;
uint32_t bitmap_size;

void pmm_init(size_t mem_size) {
    total_blocks = mem_size / BLOCK_SIZE; bitmap_size = total_blocks / BLOCKS_PER_BUCKET;
    if(bitmap_size* BLOCKS_PER_BUCKET < total_blocks)
        bitmap_size++;

    memset(bitmap, 0, bitmap_size);

    for(size_t i = 0; i < bitmap_size; i++) {
        if(bitmap[i] != 0) {
            klog(LOG_WARN, "%s: memory bitmap is not all empty, fix it!\n", __func__);
        }
    }
    klog(LOG_OK, "Physical Memory Manager initialized\n");
}

uint32_t allocate_block(void) {
    uint32_t free_block = first_free_block();
    SETBIT(free_block);
    return free_block;
}

void free_block(size_t blk_num) {
    CLEARBIT(blk_num);
}

uint32_t first_free_block(void) {
    for(size_t i = 0; i < total_blocks; i++) {
        if(!ISSET(i))
            return i;
    }
    klog(LOG_WARN, "%s: running out of free blocks\n", __func__);
    return (uint32_t) -1;
}
