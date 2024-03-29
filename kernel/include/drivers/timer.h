#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H

#include <cpu/isr.h>
#include <display.h>
#include <drivers/io_port.h>
#include <stdint.h>
#include <string.h>
#include <sys/task.h>

#define PIT_A     0x40
#define PIT_CMD   0x43

#define PIT_SCALE 1193180
#define PIT_SET   0x36

#define TIMER_IRQ 32

/* function declatations */
void timer_init(int32_t hz);
uint32_t get_ticks(void);
void sleep(uint64_t ticks);

#endif
