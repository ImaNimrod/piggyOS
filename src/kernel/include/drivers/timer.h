#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H

#include <cpu/irq.h>
#include <drivers/io_port.h>
#include <drivers/vga.h>
#include <stdint.h>

/* function declatations */
void init_timer(void);
void timer_wait(uint32_t ticks);

#endif
