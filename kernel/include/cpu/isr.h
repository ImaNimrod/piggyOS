#ifndef _KERNEL_ISR_H
#define _KERNEL_ISR_H

#include <cpu/desc_tbls.h>
#include <display.h>
#include <stdint.h>
#include <system.h>

typedef void (*isr_function)(regs_t*);

void int_install_handler(uint8_t num, isr_function handler);
void int_uninstall_handler(uint8_t num);
void isr_handler(regs_t* r);
void irq_handler(regs_t* r);

#endif
