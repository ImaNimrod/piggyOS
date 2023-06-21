#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/isr.h>
#include <cpu/irq.h>
#include <drivers/timer.h>
#include <drivers/vga.h>

void kernel_main(void) {
    init_gdt();
    init_idt();
    init_isr();
    init_irq();

    vga_clear();
    vga_set_color(VGA_COLOR_PINK, VGA_COLOR_BLACK);
    init_timer();
    timer_wait(1);
}
