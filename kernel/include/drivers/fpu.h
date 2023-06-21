#ifndef _KERNEL_FPU_H
#define _KERNEL_FPU_H

#include <display.h>
#include <drivers/io_port.h>
#include <stdint.h>

/* function declarations */
void fpu_init(void);
void fpu_set_cw(uint16_t cw);

#endif
