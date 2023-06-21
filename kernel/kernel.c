#include <cpu/cpuid.h>
#include <cpu/desc_tbls.h>
#include <display.h>
#include <drivers/ata.h>
#include <drivers/fpu.h>
#include <drivers/keyboard.h>
#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <drivers/vga.h>
#include <fs/fs.h>
#include <fs/msdospart.h>
#include <memory/kheap.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <multiboot.h>
#include <string.h>
#include <sys/spinlock.h>
#include <sys/syscall.h>
#include <sys/tasking.h>
#include <system.h>

const char* welecome_banner = "\nWelecome to:\n         _                        ___    _____    \n        (_)                      /   \\  / ____|  \n  _ __   _   __ _   __ _  _   _ |     || (___     \n | '_ \\ | | / _` | / _` || | | || | | | \\___ \\ \n | |_) || || (_| || (_| || |_| ||     | ____) |   \n | .__/ |_| \\__, | \\__, | \\__, | \\___/ |_____/\n | |         __/ |  __/ |  __/ |                  \n |_|        |___/  |___/  |___/                   \n";

void kernel_main(uint32_t mboot2_magic, mboot_info_t* mboot2_info, uint32_t inital_esp) {
    #ifdef TEXTMODE
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    #endif 

    /* initialize serial port(s) for debug output */
    serial_init();

    if(mboot2_magic != MBOOT2_MAGIC) {
        kpanic("multiboot bootloader not detected");
        return;
    }

    /* load descriptor tables and interrupt information */
    gdt_init();
    idt_init();
    tss_init(5, 0x10, 0);

    /* initalize floating-point unit */
    fpu_init();

    /* initialize memory manager systems */
    pmm_init(1096 * M);                                                             /* physical memory manager */
    vmm_init();                                                                     /* virtual memory manager */
    kheap_init(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDRESS);   /* kernel heap allocator */

    /* initialize devices */
    timer_init(100); /* programmable interrupt timer */
    keyboard_init(); /* ps/2 keyboard controller */
    rtc_init();      /* real time clock */
    ata_init();      /* ata devices */

    /* initalize multitasking */
    tasking_init(); 

    /* initialize syscalls */
    syscalls_init();

    tss_set_stack(0x10, inital_esp);

    detect_cpu();

    vga_set_color(VGA_COLOR_PINK, VGA_COLOR_BLACK);
    kprintf("%s", welecome_banner);

    for(;;);
}
