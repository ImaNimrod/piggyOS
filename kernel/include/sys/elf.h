#ifndef _KERNEL_ELF_H
#define _KERNEL_ELF_H

#include <display.h>
#include <fs/fs.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <stdint.h>

enum elf_type { 
    ELF_NONE = 0,
    ELF_REL = 1, 
    ELF_EXEC = 2
};

typedef struct {
	uint8_t magic[4]; 
    uint8_t arch;
	uint8_t byteorder;
	uint8_t elf_version;
	uint8_t abi;
	uint8_t abiversion;
	uint8_t unused[7];
	uint16_t filetype; //This contains 0, 1 or 2 - 0 is none, 1 is relocatable file, 2 is executable file.
	uint16_t machine;
	uint32_t version; //ELF file version.
	uint32_t entry; //ELF file entry point.
	uint32_t phoff; //Program header table offset in file.
	uint32_t shoff; //Section header table offset in the file.
	uint32_t flags; //ELF file flags.
	uint16_t hsize; //File header size.
	uint16_t ph_ent_size; //Program header entry size.
	uint16_t ph_ent_cnt; //Program header entry count.
	uint16_t sh_ent_size; //Section header entry size.
	uint16_t sh_ent_cnt; //Section header entry count.
	uint16_t sh_name_index; //Index in section header table with section names
} __attribute__((packed)) elf_header_t;

typedef struct {
	uint32_t s_name;
	uint32_t s_type;
	uint32_t s_flags;
	uint32_t s_addr;
	uint32_t s_offset;
	uint32_t s_size;
	uint32_t s_link;
	uint32_t s_info;
	uint32_t s_addralign;
	uint32_t s_entsize;
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
    uint32_t p_entrysize;
} __attribute__((packed)) elf_program_header_t;

/* function declarations */
uint8_t elf_check_header(elf_header_t* hdr);
elf_header_t* elf_open(fs_node_t* node);
elf_section_header_t* elf_get_section_header(elf_header_t* elf_file, uint32_t num);
elf_program_header_t* elf_get_program_header(elf_header_t* elf_file, uint32_t num);

#endif
