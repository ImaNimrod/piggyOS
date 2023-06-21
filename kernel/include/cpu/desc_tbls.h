#ifndef _KERNEL_DESC_TBLS_H
#define _KERNEL_DESC_TBLS_H

#include <stdint.h>
#include <string.h>

#define GDT_LEN 6
#define IDT_LEN 256

typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed)) gdt_descr_t;

typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

typedef struct {
    uint16_t base_low;
    uint16_t seg_sel; 
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_descr_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

typedef struct {
    uint16_t prev_tss, :16;
    uint32_t esp0;                         /* Ring 0 stack virtual address. */
    uint16_t ss0, :16;                  /* Ring 0 stack segment selector. */
    uint32_t esp1;
    uint16_t ss1, :16;
    uint32_t esp2;
    uint16_t ss2, :16;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint16_t es, :16;
    uint16_t cs, :16;
    uint16_t ss, :16;
    uint16_t ds, :16;
    uint16_t fs, :16;
    uint16_t gs, :16;
    uint16_t ldt, :16;
    uint16_t trace, bitmap;
} __attribute__((packed)) tss_entry_t;

/* function declarations */
void gdt_init(void);
void gdt_set_descr(uint16_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

void idt_init(void);
void idt_set_descr(uint8_t num, uint32_t base, uint8_t flags);

void tss_init(uint32_t idx, uint32_t kss, uint32_t kesp);
void tss_set_stack(uint32_t kss, uint32_t kesp);

#endif
