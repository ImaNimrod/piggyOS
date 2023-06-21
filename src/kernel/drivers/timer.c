#include <drivers/timer.h>

uint32_t timer_ticks = 0;

static void timer_handler(regs_t *r) {
    timer_ticks++;

    if (timer_ticks % 18 == 0) {
        vga_puts("One second has passed\n");
    }

    (void) r;
}

void timer_wait(uint32_t ticks) {
    uint64_t eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}

void init_timer(void) {
    /* installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
}

