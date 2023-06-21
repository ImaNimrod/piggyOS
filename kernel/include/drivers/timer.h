#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H

#include <cpu/irq.h>
#include <display.h>
#include <drivers/io_port.h>
#include <stdint.h>
#include <string.h>

#define PIT_A     0x40
#define PIT_CMD   0x43

#define PIT_SCALE 1193180
#define PIT_SET   0x36

#define TIMER_IRQ 0

extern void scheduler(void);
extern regs_t saved_context;

/* function declatations */
void timer_init(int32_t hz);
void sleep(uint64_t ticks);

#endif