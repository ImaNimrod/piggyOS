#include <memory/pmm.h>

static uint32_t max_blocks;
static uint32_t used_blocks;
static uint32_t mem_size;
uint8_t* pmm_bitmap = (uint8_t*) &kernel_end; 
uint32_t pmm_bitmap_size;

static void pmm_bitmap_set(uint32_t bit) {
    pmm_bitmap[bit / 8] |= (1 << (bit % 8));
    used_blocks++;
}
static void pmm_bitmap_unset(uint32_t bit) {
    pmm_bitmap[bit / 8] &= ~(1 << (bit % 8));
    used_blocks--;
}

static int pmm_bitmap_test(uint32_t bit) {
    return pmm_bitmap[bit / 8] & (1 << (bit % 8));
}

static uint32_t first_free_block(void) {
    for (uint32_t i = 0; i < max_blocks / 8; i++) {
        if (pmm_bitmap[i] != 0xff) {
            for (uint32_t j = 0; j < 8; j++) {
                if (!(pmm_bitmap[i] & (1 << j)))
                    return i * 8 + j;
            }
        }
    }

    klog(LOG_WARN, "%s: running out of free blocks\n", __func__);
    return 0;
}

uint32_t pmm_alloc_block(void) {
    if (max_blocks - used_blocks <= 0) {
        klog(LOG_WARN, "%s: kernel out of physical memory\n", __func__);
        return 0;
    }

    uint32_t free_block = first_free_block();
    pmm_bitmap_set(free_block);
    return free_block;
}

void pmm_free_block(uint32_t blk_num) {
    pmm_bitmap_unset(blk_num);
}

void pmm_init(struct mboot2_begin* mb2) {
    struct mboot2_tag* tag = mb2->tags;
    uintptr_t bitmap_addr = 0;

    /* iterate through modules and increase bitmap_addr 
     * until it points to the end of the last module */
    do {
        if (tag->type == MBOOT2_TAG_MODULE) {
            struct mboot2_tag_module* mod = (struct mboot2_tag_module*) tag;
            bitmap_addr = MAX(mod->mod_end + LOAD_MEMORY_ADDRESS, bitmap_addr);
        }  

        tag = (struct mboot2_tag*) ((uintptr_t) tag + ((tag->size + 7) & ~7));
    } while (tag->type != MBOOT2_TAG_END);

    /* modules may come before kernel */
    if (bitmap_addr < (uintptr_t) &kernel_end)
        bitmap_addr = (uintptr_t) &kernel_end;

    /* check to see if kernel and its modules are too large */
    if (bitmap_addr > LOAD_MEMORY_ADDRESS + 0x00400000)
       kpanic("kernel is too large for initial 4mb idenity mapping\n");

    struct mboot2_tag_basic_meminfo* meminfo = (struct mboot2_tag_basic_meminfo*) mboot2_find_tag(mb2, MBOOT2_TAG_BASIC_MEMINFO);
    kprintf("Low Memory size: %dkb\n", meminfo->mem_lower);
    kprintf("High Memory size: %dkb\n", meminfo->mem_upper);

    mem_size = (meminfo->mem_lower + meminfo->mem_upper) * K;
    max_blocks = mem_size / PMM_BLOCK_SIZE;

    /* place pmm_bitmap after kernel and its modules */
    pmm_bitmap = (uint8_t*) bitmap_addr;
    pmm_bitmap_size = max_blocks / 8;
    if (pmm_bitmap_size * 8 < max_blocks)
        pmm_bitmap_size++;

    memset(pmm_bitmap, 0, pmm_bitmap_size);

    klog(LOG_OK, "Physical Memory Manager initialized\n");
}
