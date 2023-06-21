#include <gdt.h>
#include <idt.h>
#include <drivers/vga.h>

void kernel_main(void) {
    init_gdt();
    init_idt();

    vga_clear();
    vga_set_color(VGA_COLOR_PINK, VGA_COLOR_BLACK);

    vga_puts("hey, pig");
}
