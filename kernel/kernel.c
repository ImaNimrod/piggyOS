#include <cpu/cpuid.h>
#include <cpu/desc_tbls.h>
#include <display.h>
#include <drivers/acpi.h>
#include <drivers/fpu.h>
#include <drivers/keyboard.h>
#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <drivers/vga.h>
#include <fs/devfs.h>
#include <fs/fs.h>
#include <fs/tarfs.h>
#include <memory/kheap.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <multiboot2.h>
#include <string.h>
#include <sys/elf.h>
#include <sys/spinlock.h>
#include <sys/syscall.h>
#include <sys/task.h>
#include <system.h>

const char* welecome_banner = "\nWelecome to:\n         _                        ___    _____    \n        (_)                      /   \\  / ____|  \n  _ __   _   __ _   __ _  _   _ |     || (___     \n | '_ \\ | | / _` | / _` || | | || | | | \\___ \\ \n | |_) || || (_| || (_| || |_| ||     | ____) |   \n | .__/ |_| \\__, | \\__, | \\__, | \\___/ |_____/\n | |         __/ |  __/ |  __/ |                  \n |_|        |___/  |___/  |___/                   \n\t\t\t\tBy: James Steffes\n";
extern void switch_to_user(void);

void kernel_main(uint32_t mboot2_magic, struct mboot2_begin* mb2, uint32_t inital_esp) {
    static uint32_t module_start, module_end;
    // static uint8_t* rsdp;

    #ifdef TEXTMODE
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    #endif 

    /* initialize serial port(s) */
    serial_init();

    /* load descriptor tables and interrupt information */
    gdt_init();
    idt_init();
    tss_init(5, 0x10, 0);

    if (mboot2_magic == MBOOT2_MAGIC) {
        if ((uint32_t) &mb2 & 7) {
            klog(LOG_ERR, "Multiboot2 info unaligned\n");
            return;
        }

        struct mboot2_tag* tag = mb2->tags;

        do {
            switch(tag->type) {
                case MBOOT2_TAG_MODULE:
                    module_start = ((struct mboot2_tag_module*) tag)->mod_start + LOAD_MEMORY_ADDRESS;
                    module_end = ((struct mboot2_tag_module*) tag)->mod_end + LOAD_MEMORY_ADDRESS;
                    break;
                // case MBOOT2_TAG_ACPI_OLD:
                //     if(!rsdp)
                //         rsdp = (uint8_t*) &((struct mboot_tag_acpi_old*) tag)->rsdp;
                //     break;
                // case MBOOT2_TAG_ACPI_NEW: 
                //     rsdp = (uint8_t*) &((struct mboot_tag_acpi_new*) tag)->rsdp;
                //     break;
                default: break;
            }

            tag = (struct mboot2_tag*) ((uintptr_t) tag + ((tag->size + 7) & ~7));
        } while (tag->type != MBOOT2_TAG_END);
    } else {
        klog(LOG_ERR, "Multiboot2 compatible bootloader not detected\n");
        return;
    }

    print_cpu_vendor();
    print_cpu_features();

    /* initialize memory manager systems */
    pmm_init(mb2);                                                                  /* physical memory manager */
    vmm_init();                                                                     /* virtual memory manager */
    kheap_init(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDRESS);   /* kernel heap allocator */

    /* initalize floating-point unit */
    fpu_init();

    // /* parse acpi tables */
    // acpi_init(rsdp);

    /* initialize devices */
    timer_init(100); /* programmable interrupt timer */
    keyboard_init(); /* ps/2 keyboard controller */
    rtc_init();      /* real time clock */

    /* initialize vfs */
    vfs_init();

    /* initialize devfs */
    devfs_init();

    /* initalize tarfs */
    tarfs_init(module_start, module_end);

    /* initialize multiasking */ 
    tasking_init();

    /* initialize syscall handler */
    syscalls_init();

    vga_set_color(VGA_COLOR_PINK, VGA_COLOR_BLACK);
    kprintf("%s\t\t\t\tVersion %d.%d (%s)\n", welecome_banner, VERSION_MAJ, VERSION_MIN, VERSION_ALIAS);

    fs_node_t* initrd = finddir_fs(get_fs_root(), "initrd"); 
    if (initrd) {
        fs_node_t* elf = finddir_fs(initrd, "initrd/test"); 
        elf_run(elf);
    }

    for(;;);
}
