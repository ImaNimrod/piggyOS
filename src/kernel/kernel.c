#include <vga.h>

void kernel_main(void) {
    clear();
    set_color(VGA_COLOR_PINK, VGA_COLOR_BLACK);
    print_string("hey, pig");
    return;
}
