#include <sys/elf.h>

uint8_t elf_check_header(elf_header_t* hdr) {
    if (hdr->magic[0] != 0x7f || 
        hdr->magic[1] != 'E' || 
        hdr->magic[2] != 'L' || 
        hdr->magic[3] != 'F') {
        klog(LOG_ERR, "Unable to load executable, ELF header not valid\n");
        return 1;
    }

    if (hdr->arch != 1) {
        klog(LOG_ERR, "Unable to load executable, not a 32-bit program\n");
        return 2;
    }

    if (hdr->byteorder != 1) {
        klog(LOG_ERR, "Unable to load executable, byte order not little-endian\n");
        return 3;
    }

    if (hdr->elf_version != 1) {
        klog(LOG_ERR, "Unable to load executable, invalid ELF header version\n");
        return 4;
    }

    if (hdr->filetype != ELF_REL && hdr->filetype != ELF_EXEC) {
        klog(LOG_ERR, "Unable to load executable, wrong file type\n");
        return 5; 
    }

    if (hdr->machine != 3) {
        klog(LOG_ERR, "Unable to load executable, program is not for i386 platform\n");
        return 6;
    }

    return 0;
}

elf_header_t* elf_open(fs_node_t* node) {
    if (!node) return NULL;

    void* addr = kmalloc(node->length);
    read_fs(node, 0, node->length, addr);
    
    elf_header_t* hdr = addr;

    if (elf_check_header(hdr))
        return NULL;

    return hdr;
}

elf_section_header_t* elf_get_section_header(elf_header_t* elf_file, uint32_t num) {
    return (elf_section_header_t*) (elf_file + elf_file->shoff + elf_file->sh_ent_size * num);
}

elf_program_header_t* elf_get_program_header(elf_header_t* elf_file, uint32_t num) {
    return (elf_program_header_t*) (elf_file + elf_file->phoff + elf_file->ph_ent_size * num);
}
