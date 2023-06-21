#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stddef.h>
#include <stdint.h>

#define GDT_LEN 5

typedef struct gdt_descr {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed)) gdt_descr_t;

typedef struct gdt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

/* function definitions */

void init_gdt(void);

#endif
