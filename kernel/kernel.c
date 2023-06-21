#include <cpu/cpuid.h>
#include <cpu/desc_tbls.h>
#include <cpu/exception.h>
#include <cpu/irq.h>
#include <display.h>
#include <drivers/ata.h>
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
#include <sys/syscall.h>
#include <sys/tasking.h>
#include <system.h>

const char* welecome_banner = "\nWelecome to:\n         _                        ___    _____    \n        (_)                      /   \\  / ____|  \n  _ __   _   __ _   __ _  _   _ |     || (___     \n | '_ \\ | | / _` | / _` || | | || | | | \\___ \\ \n | |_) || || (_| || (_| || |_| ||     | ____) |   \n | .__/ |_| \\__, | \\__, | \\__, | \\___/ |_____/\n | |         __/ |  __/ |  __/ |                  \n |_|        |___/  |___/  |___/                   \n";

void user_process() {
    while(1) 
        for(int i = 0; i < 10000; i++) 
            for(int j= 0; j < 2000; j++)
}

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
    exception_init();
    irq_init();

    /* initialize physical memory manager */
    pmm_init(1096 * M);

    /* initialize virtual memory manager */
    vmm_init();

    /* initialize kernel kheap */
    kheap_init(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDRESS);

    /* initialize devices */
    timer_init(100);
    keyboard_init();
    rtc_init();
    ata_init();

    /* initialize multitasking scheduler */
    tasking_init();

    /* initialize syscalls */
    syscalls_init();

    tss_set_stack(0x10, inital_esp);

    create_task("user_process", &user_process);
    create_task("user_process2", &user_process2);

    vga_set_color(VGA_COLOR_PINK, VGA_COLOR_BLACK);
    kprintf("%s", welecome_banner);

    for(;;);
}
