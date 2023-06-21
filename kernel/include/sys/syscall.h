#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <cpu/isr.h>
#include <display.h>
#include <stdint.h>
#include <system.h>

#define SYS_HELLO 1
#define SYS_GOODBYE 2

/* function declarations */
void syscalls_init(void);

#endif
