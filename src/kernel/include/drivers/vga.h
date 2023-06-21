#ifndef _KERNEL_VGA_H
#define _KERNEL_VGA_H

#include <stdint.h>

enum vga_color {
    VGA_COLOR_BLACK       = 0,
    VGA_COLOR_BLUE        = 1,
    VGA_COLOR_GREEN       = 2,
    VGA_COLOR_CYAN        = 3,   
    VGA_COLOR_RED         = 4,   
    VGA_COLOR_MAGENTA     = 5,  
    VGA_COLOR_BROWN       = 6,    
    VGA_COLOR_LIGHT_GRAY  = 7,
    VGA_COLOR_DARK_GRAY   = 8,
    VGA_COLOR_LIGHT_BLUE  = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN  = 11,
    VGA_COLOR_LIGHT_RED   = 12,    
    VGA_COLOR_PINK        = 13,
    VGA_COLOR_YELLOW      = 14, 
    VGA_COLOR_WHITE       = 15,
};

/* function declarations */

void vga_clear(void);
void vga_puts(const char* string);
void vga_set_color(uint8_t fg, uint8_t bg);

#endif
