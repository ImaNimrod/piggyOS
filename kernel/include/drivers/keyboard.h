#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

#include <cpu/irq.h>
#include <display.h>
#include <drivers/io_port.h>
#include <stdbool.h>
#include <stdint.h>

#define KEYBOARD_IRQ 1

/*function declarations */
void keyboard_init(void);

#endif 