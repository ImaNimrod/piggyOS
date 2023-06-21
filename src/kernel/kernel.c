#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/irq.h>
#include <cpu/isr.h>
#include <display.h>
#include <drivers/fdc.h>
#include <drivers/keyboard.h>
#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/timer.h>
#include <drivers/vga.h>
#include <memory.h>

extern uintptr_t kernel_end;

void kernel_main(void) {
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    mmu_init(&kernel_end);

    gdt_init();
    idt_init();
    isr_init();
    irq_init();

    paging_init();

    timer_init(100);
    keyboard_init();

    fdc_init();

    pci_init();
    mmu_print_state();

    for(;;) {
        __asm__("hlt");
    }
}
