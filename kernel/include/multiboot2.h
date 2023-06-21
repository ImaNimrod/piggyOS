#ifndef _KERNEL_MBOOT_H
#define _KERNEL_MBOOT_H

#include <stddef.h>
#include <stdint.h>

#define MBOOT2_MAGIC 0x36d76289

/* multiboot2 tag types */
#define MBOOT2_TAG_END               0
#define MBOOT2_TAG_CMDLINE           1
#define MBOOT2_TAG_BOOT_LOADER_NAME  2
#define MBOOT2_TAG_MODULE            3
#define MBOOT2_TAG_BASIC_MEMINFO     4
#define MBOOT2_TAG_BOOTDEV           5
#define MBOOT2_TAG_MMAP              6
#define MBOOT2_TAG_VBE               7
#define MBOOT2_TAG_FRAMEBUFFER       8
#define MBOOT2_TAG_ELF_SECTIONS      9
#define MBOOT2_TAG_APM               10
#define MBOOT2_TAG_EFI32             11
#define MBOOT2_TAG_EFI64             12
#define MBOOT2_TAG_SMBIOS            13
#define MBOOT2_TAG_ACPI_OLD          14
#define MBOOT2_TAG_ACPI_NEW          15
#define MBOOT2_TAG_NETWORK           16
#define MBOOT2_TAG_EFI_MMAP          17
#define MBOOT2_TAG_EFI_BS            18
#define MBOOT2_TAG_EFI32_IH          19
#define MBOOT2_TAG_EFI64_IH          20
#define MBOOT2_TAG_LOAD_BASE_ADDR    21

struct mboot2_tag {
    uint32_t type;
    uint32_t size;
} __attribute__((packed));

struct mboot2_begin {
    uint32_t total_size;
    uint32_t reserved;
    struct mboot2_tag tags[];
} __attribute__((packed));

struct mboot2_tag_basic_meminfo {
    struct mboot2_tag tag;
    uint32_t mem_lower;
    uint32_t mem_upper;
} __attribute__((packed));

struct mboot2_mmap_entry {
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t length_low;
    uint32_t length_high;
#define MULTIBOOT_MEMORY_AVAILABLE		        1
#define MULTIBOOT_MEMORY_RESERVED		        2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
    uint32_t type;
    uint32_t zero;
} __attribute__((packed));

struct mboot2_tag_mmap {
    struct mboot2_tag tag;
    uint32_t entry_size;
    uint32_t entry_version;
    struct mboot2_mmap_entry entries[];  
} __attribute__((packed));

struct mboot2_tag_module {
    struct mboot2_tag tag;
    uint32_t mod_start;
    uint32_t mod_end;
    char name[];
} __attribute__((packed));

struct mboot2_tag_framebuffer {
    struct mboot2_tag tag;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t type;
    uint8_t reserved;
} __attribute__((packed));

struct mboot_tag_acpi_old {
    struct mboot2_tag tag; 
	uint8_t* rsdp;
} __attribute__((packed));

struct mboot_tag_acpi_new {
    struct mboot2_tag tag; 
	uint8_t* rsdp;
} __attribute__((packed));

/* function declarations */ 
struct mboot2_tag* mboot2_find_tag(struct mboot2_begin* mb2, uint32_t tag_type);

#endif
