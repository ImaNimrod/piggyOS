#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H

#include <cpu/irq.h>
#include <drivers/io_port.h>
#include <drivers/vga.h>
#include <stdint.h>

#define PIT_A       0x40
#define PIT_CMD   0x43

#define PIT_SCALE 1193180
#define PIT_SET   0x36

#define TIMER_IRQ 0

/* function declatations */
void init_timer(void);
void timer_wait(uint32_t ticks);

#endif
