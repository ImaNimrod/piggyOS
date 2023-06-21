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

    elf_header_t* hdr = kmalloc(node->length);
    read_fs(node, 0, node->length, (uint8_t*) hdr);
    
    if (elf_check_header(hdr))
        return NULL;

    return hdr;
}

static inline elf_section_header_t* elf_get_section_header(elf_header_t* elf_file, uint32_t num) {
    return (elf_section_header_t*) (elf_file + elf_file->shoff + elf_file->sh_ent_size * num);
}

static inline elf_program_header_t* elf_get_program_header(elf_header_t* elf_file, uint32_t num) {
    return (elf_program_header_t*) (elf_file + elf_file->phoff + elf_file->ph_ent_size * num);
}

void elf_hdr_info(elf_header_t* hdr) {
    kprintf("\tHeader information:\n");
    kprintf("\t Architecture: %s\n", (hdr->arch==1) 
                                         ? "32-bit" 
                                         : (hdr->arch == 2) 
                                           ? "64-bit" 
                                           : "Unknown architecture");
    kprintf("\t Byte order: %s\n", (hdr->byteorder == 1) 
                                       ? "little-endian" 
                                       : "unknown");
    kprintf("\t ELF file version: %d\n", hdr->elf_version);
    kprintf("\t ELF file type: %s\n", (hdr->filetype == 1) 
                                          ? "relocatable" 
                                          : (hdr->filetype == 2) 
                                            ? "executable" 
                                            : "unknown");
    kprintf("\t Target machine: %s\n", (hdr->machine == 3) 
                                           ? "i386" 
                                           : "unknown");
    kprintf("\t Entry point: 0x%x\n", hdr->entry);
    kprintf("\t Program header offset: %d\n", hdr->phoff);
    kprintf("\t Section header offset: %d\n", hdr->shoff);
    kprintf("\t File flags: %d\n", hdr->flags);
    kprintf("\t File header size: %d\n", hdr->hsize);
    kprintf("\t Program header entry size: %d\n", hdr->ph_ent_size);
    kprintf("\t Section header entry size: %d\n", hdr->sh_ent_size);
    kprintf("\t Section header count: %d\n", hdr->sh_ent_cnt);
    kprintf("\t Program header count: %d\n", hdr->ph_ent_cnt);
}

int run_elf_file(elf_header_t* hdr, int argc, char **argv) {
    kprintf("loading segments:\n");
    uint32_t vmm_alloced[4096] = {0};
    int32_t ptr_vmm_alloced = 0;

    for (int32_t i = 0; i < hdr->ph_ent_cnt; i++) {
        elf_program_header_t *phdr = elf_get_program_header(hdr, i);
        if (phdr->type != 0) {
            continue;
        }
        kprintf("Loading %x bytes to %x\n", phdr->size_in_mem, phdr->load_to);

        uint32_t alloc_addr;
        for (alloc_addr = phdr->load_to;
            alloc_addr < phdr->load_to + phdr->size_in_mem;
            alloc_addr += PAGE_SIZE) {
            vmm_alloced[ptr_vmm_alloced] = alloc_addr;
            ptr_vmm_alloced++;
            kprintf("Alloc %d: %x\n", ptr_vmm_alloced, alloc_addr);
            allocate_page(kernel_page_dir, alloc_addr, 0, 0, 1);
        }

        memset((void*) phdr->load_to, 0, phdr->size_in_mem);
        memcpy((void*) phdr->load_to, hdr + phdr->data_offset, phdr->size_in_file);
        kprintf("Loaded\n");
    }

    int(*entry_point)(int argc, char** argv) = (void*) (hdr->entry);
    kprintf("ELF entry point: %x\n", hdr->entry);

    kprintf("Executing\n");
    int _result = entry_point(argc, argv);

    kprintf("[PROGRAMM FINISHED WITH CODE <%d>]\n", _result);
    kprintf("Cleaning VMM:\n");

    for (int32_t i = 0; i != ptr_vmm_alloced; i++){
        kprintf("\tCleaning %d: %x\n", i, vmm_alloced[i]);
        free_page(kernel_page_dir, vmm_alloced[i], 0);
    }
    kprintf("[CLEANED <%d> PAGES]\n", ptr_vmm_alloced);

    return 0;
}
