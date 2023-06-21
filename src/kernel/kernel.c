#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/irq.h>
#include <cpu/isr.h>
#include <display.h>
#include <drivers/ata.h>
#include <drivers/fdc.h>
#include <drivers/keyboard.h>
#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/timer.h>
#include <drivers/vga.h>
#include <memory.h>
#include <string.h>

extern uintptr_t kernel_end;

void kernel_main(void) {
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    gdt_init();
    idt_init();
    isr_init();
    irq_init();

    mmu_init(&kernel_end);
    paging_init();

    timer_init(100);
    keyboard_init();
    rtc_init();
    fdc_init();
    ata_init();

    mmu_print_state();

    for(;;) {
        __asm__("hlt");
    }
}
