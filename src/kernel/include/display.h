#ifndef _KERNEL_DISPLAY_H
#define _KERNEL_DISPLAY_H

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <drivers/vga.h>

#define kpanic(...) {kprintf("***KERNEL_PANIC*** in %s at line %d in function: %s\n", __FILE__, __LINE__, __func__); kprintf(__VA_ARGS__); for(;;);}

/* function declarations */
int kprintf (const char* str, ...);

#endif