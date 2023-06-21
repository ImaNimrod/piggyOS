#include <sys/elf.h>

uint8_t elf_check_header(elf_header_t* header) {
    if (header->e_ident[0] != 0x7f || 
        header->e_ident[1] != 'E' || 
        header->e_ident[2] != 'L' || 
        header->e_ident[3] != 'F') {
        return 0;
    }

    if (header->e_ident[EI_CLASS] != 1)
        return 0;

    if (header->e_ident[EI_DATA] != 1)
        return 0;

    if (header->e_ident[EI_VERSION] != 1)
        return 0;
    
    if (header->e_machine != 3)
        return 0;

    if (header->e_type != ET_REL && header->e_type != ET_EXEC)
        return 0;

    return 1;
}

static inline elf_section_header_t* elf_get_section_header(elf_header_t* header, uint32_t num) {
    return (elf_section_header_t*) ((uint32_t) header + header->e_shoff + num * header->e_shentsize);
}

static inline elf_program_header_t* elf_get_program_header(elf_header_t* header, uint32_t num) {
    return (elf_program_header_t*) ((uint32_t) header + header->e_phoff + num * header->e_phentsize);
}

static int elf_prep_sections(elf_header_t* header) {
    static elf_section_header_t* sheader;

    for (uint32_t i = 0; i < header->e_shnum; i++) {
        sheader = elf_get_section_header(header, i);

        // handle bss section
        if (sheader->sh_type == SHT_NOBITS) {
            if (!sheader->sh_size) continue;

            if (sheader->sh_flags & SHF_ALLOC) {
                // allocate and zero bss memory
                void* mem = kmalloc(sheader->sh_size);
                memset(mem, 0, sheader->sh_size);

                sheader->sh_offset = (uint32_t) mem - (uint32_t) header;
            }
        }
    }

    return 0;
}

static int elf_relocate(elf_header_t* header, uint32_t offset, int argc, char** argv) {
    static elf_program_header_t* pheader;

    for (uint32_t i = 0; i < header->e_phnum; i++) {
        pheader = elf_get_program_header(header, i);

        if (pheader->p_type != PT_LOAD) continue;

        kprintf("ELF PHeader #%d, Type: LOAD, Off:0x%x, Vaddr:0x%x, Size:0x%d\n", i, pheader->p_offset, pheader->p_vaddr, pheader->p_memsz);

        allocate_region(kernel_page_dir, pheader->p_vaddr, pheader->p_vaddr + pheader->p_filesz, 0, 0, 1);
        memcpy((uint8_t*) pheader->p_vaddr, (uint8_t*) offset + pheader->p_offset, pheader->p_filesz);

        if (pheader->p_filesz < pheader->p_memsz)
            memset((uint8_t*) pheader->p_vaddr + pheader->p_filesz, 0, pheader->p_memsz - pheader->p_filesz);
    }


    typedef int (*entry_point) (int, char**);
    entry_point entry = (entry_point) header->e_entry;

    int result = entry(argc, argv);

    kprintf("Program finished with value: %d", result);

    return 0;
}

int elf_run(fs_node_t* node) {
    if (!node)
        return 1;

    open_fs(node, 0);

    uint8_t* modulebuffer = (uint8_t*) kmalloc(node->length);
    read_fs(node, 0, node->length, modulebuffer);

    if (!elf_check_header((elf_header_t*) modulebuffer))
        return 1;

    if (elf_prep_sections((elf_header_t*) modulebuffer)) {
        klog(LOG_ERR, "Failed to prepare ELF section\n");
    }

    if (elf_relocate((elf_header_t*) modulebuffer, (uint32_t) modulebuffer, 0, NULL)) {
        klog(LOG_ERR, "Failed to relocate ELF binary\n");
    }

    close_fs(node);
    kfree(modulebuffer);

    return 0;
}
