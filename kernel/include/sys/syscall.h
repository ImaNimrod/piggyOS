#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <cpu/isr.h>
#include <display.h>
#include <stdint.h>
#include <system.h>
#include <sys/task.h>

#define SYS_GETPID 1

/* function declarations */
void syscalls_init(void);

#endif
