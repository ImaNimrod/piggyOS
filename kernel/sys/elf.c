#include <sys/elf.h>

extern page_directory_t* current_page_dir;

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

    return -1;
}

static inline elf_section_header_t* elf_get_section_header(elf_header_t* header, uint32_t num) {
    return (elf_section_header_t*) ((uint32_t) header + header->e_shoff + num * header->e_shentsize);
}

static inline elf_program_header_t* elf_get_program_header(elf_header_t* header, uint32_t num) {
    return (elf_program_header_t*) ((uint32_t) header + header->e_phoff + num * header->e_phentsize);
}

static int elf_relocate(elf_header_t* header, uint32_t offset, int argc, char** argv) {
    static elf_program_header_t* pheader;

    for (uint32_t i = 0; i < header->e_phnum; i++) {
        pheader = elf_get_program_header(header, i);

        if (pheader->p_type != PT_LOAD) continue;

        if (pheader->p_memsz < pheader->p_filesz || KERN_BASE <= pheader->p_vaddr + pheader->p_memsz)
            return -1;

        kprintf("ELF PHeader #%d, Type: LOAD, Off:0x%x, Vaddr:0x%x, Size:0x%d\n", i, pheader->p_offset, pheader->p_vaddr, pheader->p_memsz);

        allocate_region(current_page_dir, pheader->p_vaddr, pheader->p_vaddr + pheader->p_filesz, 0, 0, 1);
        memcpy((uint8_t*) pheader->p_vaddr, (uint8_t*) offset + pheader->p_offset, pheader->p_filesz);

        if (pheader->p_memsz - pheader->p_filesz > 0)
            memset((uint8_t*) pheader->p_vaddr + pheader->p_filesz, 0, pheader->p_memsz - pheader->p_filesz);
    }

    return 0;
}

int elf_load(fs_node_t* node) {
    if (!node)
        return -1;

    int ret = 0;

    open_fs(node, 0);

    uint8_t* modulebuffer = (uint8_t*) kmalloc(node->length);
    read_fs(node, 0, node->length, modulebuffer);

    if (!elf_check_header((elf_header_t*) modulebuffer)) {
        klog(LOG_ERR, "ELF binary is not for this machine\n");
        ret = -1;
    }

    if (elf_relocate((elf_header_t*) modulebuffer, (uint32_t) modulebuffer, 0, NULL)) {
        klog(LOG_ERR, "Failed to relocate ELF binary\n");
        ret = -1;
    }

    close_fs(node);
    kfree(modulebuffer);

    return ret;
}
