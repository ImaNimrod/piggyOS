#include <cpu/isr.h>
#include <sys/task.h>

extern task_t* current_task;
static isr_function interrupt_handlers[256];

void int_install_handler(uint8_t num, isr_function handler) {
    interrupt_handlers[num] = handler;
}

void int_unintstall_handler(uint8_t num) {
    interrupt_handlers[num] = 0;
}

void isr_handler(regs_t* r) {
    current_task->ifr = r;

    if(interrupt_handlers[r->int_no] != 0) {
        isr_function handler = interrupt_handlers[r->int_no];
        handler(r);
        return;
    }

    klog(LOG_WARN, "Unhandled interrupt: %d at 0x%x\n", r->int_no, r->eip);
}

void irq_handler(regs_t* r) {
    current_task->ifr = r;

    if (r->int_no >= 40) {
        outb(0xa0, 0x20);
    }
    outb(0x20, 0x20);

    if (interrupt_handlers[r->int_no] != 0) {
        isr_function handler = interrupt_handlers[r->int_no];
        handler(r);
    }
}
