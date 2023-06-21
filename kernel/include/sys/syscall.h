#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <cpu/isr.h>
#include <display.h>
#include <stdint.h>
#include <system.h>
#include <sys/task.h>

#define SYS_GETTID      1
#define SYS_YIELD       2
#define SYS_EXIT        3
#define SYS_RPL3TEST    4

extern int sys_gettid(void);
extern int sys_yield(void);
extern int sys_exit(void);

/* function declarations */
void syscalls_init(void);

#endif
