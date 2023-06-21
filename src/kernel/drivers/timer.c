#include <drivers/timer.h>

uint32_t timer_ticks = 0;

static void timer_phase(uint32_t hz) {
	int divisor = PIT_SCALE / hz;
	outb(PIT_CMD, PIT_SET);
	outb(PIT_A, divisor & 0xFF);
 	outb(PIT_A, (divisor >> 8) & 0xFF);
}

static void timer_handler(regs_t *r) {
    timer_ticks++;
    irq_ack(TIMER_IRQ);

    (void) r;
}

void timer_wait(uint32_t ticks) {
    uint64_t eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}

void timer_init(void) {
    /* installs 'timer_handler' to IRQ0 */
    vga_puts("initializing PIT...\n");
    irq_install_handler(TIMER_IRQ, timer_handler);
    timer_phase(100);
}

