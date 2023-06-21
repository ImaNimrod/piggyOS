#ifndef _KERNEL_MBOOT_H
#define _KERNEL_MBOOT_H

#include <stdint.h>

#define MBOOT2_MAGIC 0x36d76289

#define MBOOT_TAG_ALIGN                  8
#define MBOOT_TAG_TYPE_END               0
#define MBOOT_TAG_TYPE_CMDLINE           1
#define MBOOT_TAG_TYPE_BOOT_LOADER_NAME  2
#define MBOOT_TAG_TYPE_MODULE            3
#define MBOOT_TAG_TYPE_BASIC_MEMINFO     4
#define MBOOT_TAG_TYPE_BOOTDEV           5
#define MBOOT_TAG_TYPE_MMAP              6
#define MBOOT_TAG_TYPE_VBE               7
#define MBOOT_TAG_TYPE_FRAMEBUFFER       8
#define MBOOT_TAG_TYPE_ELF_SECTIONS      9
#define MBOOT_TAG_TYPE_APM               10
#define MBOOT_TAG_TYPE_EFI32             11
#define MBOOT_TAG_TYPE_EFI64             12
#define MBOOT_TAG_TYPE_SMBIOS            13
#define MBOOT_TAG_TYPE_ACPI_OLD          14
#define MBOOT_TAG_TYPE_ACPI_NEW          15
#define MBOOT_TAG_TYPE_NETWORK           16
#define MBOOT_TAG_TYPE_EFI_MMAP          17
#define MBOOT_TAG_TYPE_EFI_BS            18
#define MBOOT_TAG_TYPE_EFI32_IH          19
#define MBOOT_TAG_TYPE_EFI64_IH          20
#define MBOOT_TAG_TYPE_LOAD_BASE_ADDR    21

typedef struct {
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
#define MULTIBOOT_MEMORY_AVAILABLE		        1
#define MULTIBOOT_MEMORY_RESERVED		        2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
    uint32_t type;
    uint32_t zero;
} mboot_mmap_t;

typedef struct {
    uint32_t type;
    uint32_t size;
} mboot_tag_t;

struct mboot_tag_basic_meminfo {
    mboot_tag_t tag;
    uint32_t mem_lower;
    uint32_t mem_upper;
};

struct mboot_tag_mmap {
    mboot_tag_t tag;
    uint32_t entry_size;
    uint32_t entry_version;
    mboot_mmap_t entries[];  
};

struct mboot_tag_module {
    mboot_tag_t tag;
    uint32_t mod_start;
    uint32_t mod_end;
    char cmdline;
};

struct mboot_tag_old_acpi {
    mboot_tag_t tag;
	uint8_t* rsdp;
};

struct mboot_tag_new_acpi {
    mboot_tag_t tag;
	uint8_t* rsdp;
};

#endif
