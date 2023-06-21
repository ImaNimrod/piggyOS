#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/irq.h>
#include <cpu/isr.h>
#include <display.h>
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

    timer_init();
    keyboard_init();

    pci_init();

    mmu_print_state();

    time_t time;
    get_time(&time);
    kprintf("%d %d/%d %d:%d:%d\n", time.year + 2000, time.mnth, time.day, time.hour, time.min, time.sec);

    for(;;) {
        __asm__("hlt");
    }
}
