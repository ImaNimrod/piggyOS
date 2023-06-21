#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <cpu/exception.h>
#include <display.h>
#include <stdint.h>
#include <system.h>
#include <sys/tasking.h>

/* tasking.c */
extern regs_t saved_context;

/* function declarations */
void syscalls_init(void);

#endif
