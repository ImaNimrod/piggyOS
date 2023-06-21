#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <cpu/isr.h>
#include <display.h>
#include <stdint.h>
#include <system.h>
#include <sys/task.h>

#define SYS_GETPID      1
#define SYS_YIELD       2
#define SYS_EXIT        3
#define SYS_OPEN        4
#define SYS_CLOSE       5
#define SYS_READ        6
#define SYS_WRITE       7

/* function declarations */
void syscalls_init(void);

#endif
