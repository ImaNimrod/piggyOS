#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

#include <cpu/isr.h>
#include <display.h>
#include <drivers/io_port.h>
#include <stdint.h>

#define PS2_DATA 0x60
#define PS2_CMD  0x64

#define KEYBOARD_IRQ 33

#define SHIFT (1<<0)
#define CTL   (1<<1)
#define ALT   (1<<2)

#define CAPSLOCK   (1<<3)
#define NUMLOCK    (1<<4)
#define SCROLLLOCK (1<<5)

#define E0ESC (1<<6)

#define C(x) (x - '@')

/*function declarations */
void keyboard_init(void);

#endif 
