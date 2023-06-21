#ifndef _KERNEL_VMM_H
#define _KERNEL_VMM_H

#include <cpu/isr.h>
#include <display.h>
#include <memory/kheap.h>
#include <memory/pmm.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <system.h>

#define PAGE_SIZE 4096

#define ALIGN(addr) (((uint32_t) (addr) & 0xFFFFF000) + PAGE_SIZE)
#define IS_ALIGN(addr) ((((uint32_t)(addr)) | 0xFFFFF000) == 0)

#define PAGEDIR_INDEX(va) (((uint32_t) va) >> 22)
#define PAGETBL_INDEX(va) ((((uint32_t) va) >> 12) & 0x3ff)
#define PAGEFRAME_INDEX(va) (((uint32_t) va) & 0xfff)

#define SET_PGBIT(cr0) (cr0 = cr0 | 0x80000000)
#define CLEAR_PSEBIT(cr4) (cr4 = cr4 & 0xffffffef)

typedef struct {
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;
    uint32_t w_through  : 1;
    uint32_t cache      : 1;
    uint32_t access     : 1;
    uint32_t reserved   : 1;
    uint32_t page_size  : 1;
    uint32_t global     : 1;
    uint32_t available  : 3;
    uint32_t frame      : 20;
} page_dir_entry_t;

typedef struct {
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;
    uint32_t reserved   : 2;
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;
    uint32_t reserved2  : 2;
    uint32_t available  : 3;
    uint32_t frame      : 20;
} page_table_entry_t;

typedef struct {
    page_table_entry_t pages[1024];
} page_table_t;

typedef struct {
    page_dir_entry_t tables[1024];
    page_table_t* ref_tables[1024];
} page_directory_t;

// boot_page_dir defined in entry.asm
extern page_directory_t* boot_page_dir;

// kernel_page_dir defined in memory/vmm.c 
extern page_directory_t* kernel_page_dir;

// pmm_bitmap defined in pmm.c
extern uint8_t* pmm_bitmap;
extern uint32_t pmm_bitmap_size;

/* function declarations */
void* virt2phys(page_directory_t* dir, void* virtual_addr);
void vmm_init(void);
void page_fault(regs_t *r);
void* ksbrk(int32_t size);
void allocate_page(page_directory_t* dir, uint32_t virtual_addr, uint32_t frame, int is_kernel, int is_writable);
void allocate_region(page_directory_t *dir, uint32_t start_va, uint32_t end_va, int iden_map, int is_kernel, int is_writable);
void free_page(page_directory_t* dir, uint32_t virtual_addr, int free);
void free_region(page_directory_t* dir, uint32_t start_va, uint32_t end_va, int free);
void switch_page_directory(page_directory_t* page_dir, uint32_t phys);
void copy_page_directory(page_directory_t* dst, page_directory_t* src);

#endif
