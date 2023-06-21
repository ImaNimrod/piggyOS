#include <cpu/cpuid.h>
#include <cpu/desc_tbls.h>
#include <display.h>
#include <drivers/fpu.h>
#include <drivers/keyboard.h>
#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <drivers/vga.h>
#include <fs/devfs.h>
#include <fs/ext2.h>
#include <fs/fs.h>
#include <memory/kheap.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <multiboot2.h>
#include <string.h>
#include <sys/spinlock.h>
#include <sys/syscall.h>
#include <sys/tasking.h>
#include <system.h>

const char* welecome_banner = "\nWelecome to:\n         _                        ___    _____    \n        (_)                      /   \\  / ____|  \n  _ __   _   __ _   __ _  _   _ |     || (___     \n | '_ \\ | | / _` | / _` || | | || | | | \\___ \\ \n | |_) || || (_| || (_| || |_| ||     | ____) |   \n | .__/ |_| \\__, | \\__, | \\__, | \\___/ |_____/\n | |         __/ |  __/ |  __/ |                  \n |_|        |___/  |___/  |___/                   \n";

void kernel_main(uint32_t mboot2_magic, uint32_t mboot2_info, uint32_t inital_esp) {
    uint32_t module_start = 0;
    uint32_t module_end = 0;
    static struct mboot_tag_basic_meminfo* meminfo;

    #ifdef TEXTMODE
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    #endif 

    /* initialize serial port(s) for debug output */
    serial_init();

    if(mboot2_magic == MBOOT2_MAGIC) {
        if(mboot2_info & 7) {
            klog(LOG_ERR, "Multiboot2 info unaligned\n");
            return;
        }

        for (mboot_tag_t* tag = (mboot_tag_t*) ((uint32_t) mboot2_info + 8);
                tag->type != MBOOT_TAG_TYPE_END;
                tag = (mboot_tag_t*) ((uint8_t*) tag + ((tag->size + 7) & ~7)))
        { 
            switch(tag->type) {
                case MBOOT_TAG_TYPE_MODULE:
                    module_start = ((struct mboot_tag_module*) tag)->mod_start + LOAD_MEMORY_ADDRESS;
                    module_end = ((struct mboot_tag_module*) tag)->mod_end + LOAD_MEMORY_ADDRESS;
                    break;
                case MBOOT_TAG_TYPE_BASIC_MEMINFO:
                    meminfo = (struct mboot_tag_basic_meminfo*) tag;
                    break;
            }
        }
    } else {
        klog(LOG_ERR, "Multiboot2 compatible bootloader not detected\n");
        return;
    }

    /* load descriptor tables and interrupt information */
    gdt_init();
    idt_init();
    tss_init(5, 0x10, 0);

    /* initalize floating-point unit */
    fpu_init();

    /* initialize memory manager systems */
    pmm_init(meminfo);                                                              /* physical memory manager */
    vmm_init();                                                                     /* virtual memory manager */
    kheap_init(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDRESS);   /* kernel heap allocator */

    /* initialize devices */
    timer_init(100); /* programmable interrupt timer */
    keyboard_init(); /* ps/2 keyboard controller */
    rtc_init();      /* real time clock */

    allocate_region(kernel_page_dir, 0xd0000000, (0xd0000000 + (module_end - module_start)), 0, 1, 1);
    char* ramdisk = (char*) 0xd0000000;
    memmove(ramdisk, (char*) module_start, (module_end - module_start));

    if (ramdisk && !fs_root) 
		ext2_ramdisk_mount((uintptr_t) ramdisk);

    /* initalize vfs */
    vfs_init();

    if (!fs_root) {
        klog(LOG_ERR, "Mounting ramdisk as root FAILED\n");
        return;
    }

    devfs_init();

    null_device_create();
    port_device_create();
    zero_device_create();

    /* initalize multitasking */
    tasking_init(); 

    /* initialize syscalls */
    syscalls_init();

    tss_set_stack(0x10, inital_esp);

    vga_set_color(VGA_COLOR_PINK, VGA_COLOR_BLACK);
    kprintf("%s", welecome_banner);

    for(;;);
}
