#ifndef _KERNEL_ELF_H
#define _KERNEL_ELF_H

#include <display.h>
#include <fs/fs.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/task.h>

enum elf_ident {
    EI_MAG0        = 0,
    EI_MAG1        = 1,
    EI_MAG2        = 2,
    EI_MAG3        = 3,
    EI_CLASS       = 4,
    EI_DATA        = 5, 
    EI_VERSION     = 6, 
    EI_OSABI       = 7, 
    EI_ABIVERSION  = 8, 
    EI_PAD         = 9,
};

enum elf_type {
    ET_NONE = 0,
    ET_REL  = 1,
    ET_EXEC = 2,
};

#define PT_NULL             0
#define PT_LOAD             1
#define PT_DYNAMIC          2
#define PT_INTERP           3
#define PT_NOTE             4
#define PT_SHLIB            5
#define PT_PHDR             6
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff

#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_NOBITS   8
#define SHT_REL      9

#define SHF_WRITE     0x1
#define SHF_ALLOC     0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKPROC  0xf0000000

typedef struct {
    uint8_t e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} __attribute__((packed)) elf_header_t;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} __attribute__((packed)) elf_section_header_t;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} __attribute__((packed)) elf_program_header_t;

/* function declarations */
uint8_t elf_check_header(elf_header_t* hdr);
int elf_run(fs_node_t* node);

#endif
