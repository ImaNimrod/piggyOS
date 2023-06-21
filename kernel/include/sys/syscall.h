#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <cpu/isr.h>
#include <display.h>
#include <stdint.h>
#include <system.h>

/* defined in tasking.c */
extern regs_t saved_context;

/* function declarations */
void syscalls_init(void);

#endif
