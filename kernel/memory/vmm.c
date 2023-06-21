#include <memory/vmm.h>

extern void *heap_start, *heap_end, *heap_max, *heap_curr;
extern int kheap_enabled;

uint8_t *temp_mem;
bool paging_enabled = false;

page_directory_t* kernel_page_dir = {0};
page_directory_t* current_page_dir = {0};

static void *dumb_kmalloc(uint32_t size, int align) {
    void *ret = temp_mem;
    if(align && !IS_ALIGN(ret))
        ret = (void*) ALIGN(ret);
    temp_mem = temp_mem + size;
    return ret;
}

void *virt2phys(page_directory_t* dir, void* virtual_addr) {
    if(!paging_enabled) {
        return (void*) ((char*) virtual_addr - LOAD_MEMORY_ADDRESS);
    }
    uint32_t page_dir_idx = PAGEDIR_INDEX(virtual_addr), page_tbl_idx = PAGETBL_INDEX(virtual_addr), page_frame_offset = PAGEFRAME_INDEX(virtual_addr);
    if(!dir->ref_tables[page_dir_idx]) {
        kprintf("%s: page dir entry does not exist\n", __func__);
        return NULL;
    }
    page_table_t* table = dir->ref_tables[page_dir_idx];
    if(!table->pages[page_tbl_idx].present) {
        kprintf("%s: page table entry does not exist\n", __func__);
        return NULL;
    }
    uint32_t t = table->pages[page_tbl_idx].frame;
    t = (t << 12) + page_frame_offset;
    return (void*) t;
}

static void allocate_page(page_directory_t *dir, uint32_t virtual_addr, uint32_t frame, int is_kernel, int is_writable) {
    page_table_t *table = NULL;
    if(!dir) {
        kprintf("%s: page directory is empty\n", __func__);
        return;
    }

    uint32_t page_dir_idx = PAGEDIR_INDEX(virtual_addr), page_tbl_idx = PAGETBL_INDEX(virtual_addr);
    table = dir->ref_tables[page_dir_idx];
    if(!table) {
        if(!kheap_enabled)
            table = dumb_kmalloc(sizeof(page_table_t), 1);
        else
            table = (page_table_t*) kmalloc_a(sizeof(page_table_t));

        memset(table, 0, sizeof(page_table_t));

        uint32_t t = (uint32_t) virt2phys(kernel_page_dir, table);
        dir->tables[page_dir_idx].frame = t >> 12;
        dir->tables[page_dir_idx].present = 1;
        dir->tables[page_dir_idx].rw = 1;
        dir->tables[page_dir_idx].user = 1;
        dir->tables[page_dir_idx].page_size = 0;
        dir->ref_tables[page_dir_idx] = table;
    }

    if(!table->pages[page_tbl_idx].present) {
        uint32_t t;
        if(frame)
            t = frame;
        else
            t = allocate_block();
        table->pages[page_tbl_idx].frame = t;
        table->pages[page_tbl_idx].present = 1;
        table->pages[page_tbl_idx].rw = 1;
        table->pages[page_tbl_idx].user = 1;
    }
}

void allocate_region(page_directory_t *dir, uint32_t start_va, uint32_t end_va, int iden_map, int is_kernel, int is_writable) {
    uint32_t start = start_va & 0xfffff000;
    uint32_t end = end_va & 0xfffff000;

    while(start <= end) {
        if(iden_map)
            allocate_page(dir, start, start / PAGE_SIZE, is_kernel, is_writable);
        else
            allocate_page(dir, start, 0, is_kernel, is_writable);
        start = start + PAGE_SIZE;
    }
}

static void free_page(page_directory_t* dir, uint32_t virtual_addr, int free) {
    if(dir == boot_page_dir) return;
    uint32_t page_dir_idx = PAGEDIR_INDEX(virtual_addr), page_tbl_idx = PAGETBL_INDEX(virtual_addr);
    if(!dir->ref_tables[page_dir_idx]) {
        kprintf("free_page: page dir entry does not exist\n");
        return;
    }

    page_table_t* table = dir->ref_tables[page_dir_idx];
    if(!table->pages[page_tbl_idx].present) {
        kprintf("free_page: page table entry does not exist\n");
        return;
    }

    if(free)
        free_block(table->pages[page_tbl_idx].frame);
    table->pages[page_tbl_idx].present = 0;
    table->pages[page_tbl_idx].frame = 0;
}

void free_region(page_directory_t* dir, uint32_t start_va, uint32_t end_va, int free) {
    uint32_t start = start_va & 0xfffff000;
    uint32_t end = end_va & 0xfffff000;
    while(start <= end) {
        free_page(dir, start, 1);
        start = start + PAGE_SIZE;
    }
}

void switch_page_directory(page_directory_t* page_dir, uint32_t phys) {
    uint32_t t;
    if(!phys)
        t = (uint32_t) virt2phys(boot_page_dir, page_dir);
    else
        t = (uint32_t) page_dir;
    __asm__ volatile("mov %0, %%cr3" :: "r"(t));
}

page_table_t* copy_page_table(page_directory_t* src_page_dir, page_directory_t* dst_page_dir, uint32_t page_dir_idx, page_table_t* src) {
    page_table_t* table = (page_table_t*) kmalloc_a(sizeof(page_table_t));
    for(int i = 0; i < 1024; i++) {
        if(!table->pages[i].frame)
            continue;
        uint32_t src_virtual_address = (page_dir_idx << 22) | (i << 12) | (0);
        uint32_t dst_virtual_address = src_virtual_address;
        uint32_t tmp_virtual_address = 0;

        allocate_page(dst_page_dir, dst_virtual_address, 0, 0, 1);
        allocate_page(src_page_dir, tmp_virtual_address, (uint32_t) virt2phys(dst_page_dir, (void*)dst_virtual_address), 0, 1);
        if (src->pages[i].present)  table->pages[i].present = 1;
        if (src->pages[i].rw)       table->pages[i].rw = 1;
        if (src->pages[i].user)     table->pages[i].user = 1;
        if (src->pages[i].accessed) table->pages[i].accessed = 1;
        if (src->pages[i].dirty)    table->pages[i].dirty = 1;
        memcpy((void*) tmp_virtual_address, (void*) src_virtual_address, PAGE_SIZE);

        free_page(src_page_dir, tmp_virtual_address, 0);
    }
    return table;
}

void copy_page_directory(page_directory_t* dst, page_directory_t* src) {
    for(uint32_t i = 0; i < 1024; i++) {
        if(kernel_page_dir->ref_tables[i] == src->ref_tables[i]) {
            dst->tables[i] = src->tables[i];
            dst->ref_tables[i] = src->ref_tables[i];
        } else {
            dst->ref_tables[i] = (page_table_t*) copy_page_table(src, dst, i, src->ref_tables[i]);
            uint32_t phys = (uint32_t) virt2phys(src, dst->ref_tables[i]);
            dst->tables[i].frame = phys >> 12;
            dst->tables[i].user = 1;
            dst->tables[i].rw = 1;
            dst->tables[i].present = 1;
        }
    }
}

static void enable_paging(void) {
    uint32_t cr0, cr4;

    __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
    CLEAR_PSEBIT(cr4);
    __asm__ volatile("mov %0, %%cr4" :: "r"(cr4));

    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    SET_PGBIT(cr0);
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));

    paging_enabled = true;
    klog(LOG_OK, "Paging enabled\n");
}

void vmm_init(void) {
    temp_mem = phys_mem_bitmap + phys_mem_bitmap_size;

    kernel_page_dir = dumb_kmalloc(sizeof(page_directory_t), 1);
    memset(kernel_page_dir, 0, sizeof(page_directory_t));

    uint32_t i = LOAD_MEMORY_ADDRESS;
    while(i < LOAD_MEMORY_ADDRESS + 4 * M) { /* map kernel 4Mb */
        allocate_page(kernel_page_dir, i, 0, 1, 1);
        i = i + PAGE_SIZE;
    }
    i = LOAD_MEMORY_ADDRESS + 4 * M;
    while(i < LOAD_MEMORY_ADDRESS + 4 * M + KHEAP_INITIAL_SIZE) {
        allocate_page(kernel_page_dir, i, 0, 1, 1);
        i = i + PAGE_SIZE;
    }

    switch_page_directory(kernel_page_dir, 0);

    allocate_region(kernel_page_dir, 0, 0x10000, 0, 1, 1);

    enable_paging();

	int_install_handler(14, &page_fault);

    klog(LOG_OK, "Virtual Memory Manager initialized\n");
}

void *ksbrk(int32_t size) {
    void* runner = NULL;
    void* new_boundary;
    void* old_heap_curr;
restart_sbrk:
    old_heap_curr = heap_curr;
    if(size == 0) {
        goto ret;
    } else if(size > 0) {
        new_boundary = (char*) heap_curr + size;

        if(new_boundary <= heap_end)
            goto update_boundary;
        else if(new_boundary > heap_max)
            return NULL;
        else if(new_boundary > heap_end) {
            runner = heap_end;

            while(runner < new_boundary) {
                allocate_page(kernel_page_dir, (uint32_t) runner, 0, 1, 1);
                runner = (char*) runner +  PAGE_SIZE;
            }

            if(old_heap_curr != heap_curr) {
                goto restart_sbrk;
            }
            heap_end = runner;
            goto update_boundary;
        }
    } else if(size < 0){
        new_boundary = (void*) ((uint32_t) heap_curr - size);
        if((char*) new_boundary < (char*) heap_start + KHEAP_MIN_SIZE) {
            new_boundary = (char*) heap_start + KHEAP_MIN_SIZE;
        }

        runner = (char*) heap_end - PAGE_SIZE;
        while(runner > new_boundary) {
            free_page(kernel_page_dir, (uint32_t) runner, 1);
            runner = (char*) runner - PAGE_SIZE;
        }
        heap_end = (char*) runner + PAGE_SIZE;
        goto update_boundary;
    }
update_boundary:
    heap_curr = new_boundary;
ret:
    return old_heap_curr;
}

void page_fault(regs_t *r) {
    uint32_t faulting_address;
    __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));
    
    int pres = !(r->err_code & 0x1);        // page not present
    int rw   = r->err_code & 0x2;           // write operation?
    int us   = r->err_code & 0x4;           // processor was in user-mode?
    int res  = r->err_code & 0x8;           // overwritten CPU-reserved bits of page entry?

    kprintf("page fault (");
    if (pres) { kprintf("present "); }
    if (rw) { kprintf("read-only "); }
    if (us) { kprintf("user-mode "); }
    if (res) { kprintf("reserved "); }

    kprintf("0x%x)\n", faulting_address);
}
