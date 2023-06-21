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
	uint16_t filetype;
	uint16_t machine;
	uint32_t version;
	uint32_t entry;
	uint32_t phoff;
	uint32_t shoff;
	uint32_t flags;
	uint16_t hsize;
	uint16_t ph_ent_size;
	uint16_t ph_ent_cnt;
	uint16_t sh_ent_size;
	uint16_t sh_ent_cnt;
	uint16_t sh_name_index;
} __attribute__((packed)) elf_header_t;

typedef struct {
	uint32_t	name; //Offset in section name table.
	uint32_t	type; //Section type.
	uint32_t	flags; //Section flags.
	uint32_t	addr;
	uint32_t	offset; //Actual section data offset in the file.
	uint32_t	size;
	uint32_t	link;
	uint32_t	info;
	uint32_t	addralign;
	uint32_t	entsize;
} __attribute__((packed)) elf_section_header_t;

typedef struct {
	uint32_t type; //Segment type: 0 - null, 1 - load to load_to address, 2 - requires dynamic linking, 3 - use interpreter, 4 - note.
	uint32_t data_offset; //Segment data offset in the file.
	uint32_t load_to; //Address in (virtual) memory on which segment data should be loaded to.
	uint32_t undefined; //Undefined when using System V ABI.
	uint32_t size_in_file; //Segment size in file.
	uint32_t size_in_mem; //Segment size in memory.
	uint32_t flags; //Flags: 1 - executable, 2 - writable, 4 - readable.
	uint32_t align; //Alignment, must be power of 2.
} __attribute__((packed)) elf_program_header_t;

/* function declarations */
uint8_t elf_check_header(elf_header_t* hdr);
elf_header_t* elf_open(fs_node_t* node);
void elf_hdr_info(elf_header_t* hdr);
int run_elf_file(elf_header_t* hdr, int argc, char** argv);

#endif
