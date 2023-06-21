#ifndef _KERNEL_FPU_H
#define _KERNEL_FPU_H

#include <cpu/cpuid.h>
#include <display.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/task.h>

/* function declarations */
void fpu_init(void);

#endif
