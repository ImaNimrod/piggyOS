#include <drivers/timer.h>

uint32_t timer_ticks = 0;

void timer_irq_handler(regs_t *r) {
    timer_ticks++;

    (void) r;
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

void sleep(uint64_t ticks) {
    uint64_t eticks = 0;
    eticks = ticks + timer_ticks;
    do {} while(eticks > timer_ticks);
}

#pragma GCC pop_options

void timer_init(int32_t hz) {
    int divisor = 1193180 / hz;	   
	outb(PIT_CMD, 0x36);			 
	outb(PIT_A, divisor & 0xFF);   
	outb(PIT_A, divisor >> 8);

    klog(LOG_OK, "PIT initialized\n");
}

