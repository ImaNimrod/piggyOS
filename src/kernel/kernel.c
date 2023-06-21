#include <vga.h>

void kernel_main(void) {
    vga_clear();
    vga_set_color(VGA_COLOR_PINK, VGA_COLOR_BLACK);
    vga_puts("hey, pig");
}
