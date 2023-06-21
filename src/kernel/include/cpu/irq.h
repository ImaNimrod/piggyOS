#ifndef _KERNEL_IRQ_H
#define _KERNEL_IRQ_H

#include <cpu/idt.h>
#include <drivers/io_port.h>
#include <stdint.h>

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_CMD	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_CMD	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_READ_IRR 0x0a       /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b       /* OCW3 irq service next CMD read */

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04	/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08	/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C	/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */
 
#define PIC_EOI		0x20		/* End-of-interrupt command code */

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
void irq_install(int32_t irq, void (*handler)(struct regs *r));
void irq_uninstall(int32_t irq);

extern void irq_handler(struct regs *r);

#endif 
