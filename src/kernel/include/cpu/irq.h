#ifndef _KERNEL_IRQ_H
#define _KERNEL_IRQ_H

#include <cpu/idt.h>
#include <drivers/io_port.h>
#include <stdint.h>

#define PIC1_CMD   0x20
#define PIC1_DATA  PIC1_CMD+1
#define PIC2_CMD   0xA0
#define PIC2_DATA  PIC1_CMD+1

#define PIC_EOI    0x20

/* function declarations */ 
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void init_irq(void);
void irq_install_handler(uint8_t irq, void (*handler)(regs_t *r));
void irq_uninstall_handler(uint8_t irq);
void irq_ack(uint8_t irq);

void irq_handler(regs_t *r);

#endif 
