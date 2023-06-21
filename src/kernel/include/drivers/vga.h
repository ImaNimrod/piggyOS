#ifndef _KERNEL_VGA_H
#define _KERNEL_VGA_H

#include <drivers/io_port.h>
#include <stdint.h>
#include <string.h>

#define NUM_COLS 80
#define NUM_ROWS 25

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
void vga_putc(const char c);
void vga_puts(const char* string);
void vga_set_color(enum vga_color fg, enum vga_color bg);

#endif
