#include <cpu/gdt.h>

static gdt_descr_t gdt[GDT_LEN];
static gdt_ptr_t gdt_ptr;

static void gdt_set_descr(gdt_descr_t* descr, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    descr->base_low = (base & 0xFFFF);
    descr->base_middle = (base >> 16) & 0xFF;
    descr->base_high = (base >> 24) & 0xFF;
    descr->limit_low = (limit & 0xFFFF);
    descr->granularity = ((limit >> 16) & 0x0F) | (granularity & 0xF0);
    descr->access = access;
}

static void gdt_set(gdt_ptr_t* gdt_ptr) {
    __asm__ volatile("lgdtl (gdt_ptr)");
    __asm__ volatile("movw $0x10, %ax    \n \
                      movw %ax, %ds    \n \
                      movw %ax, %es    \n \
                      movw %ax, %fs    \n \
                      movw %ax, %gs    \n \
                      ljmp $0x08, $next    \n \
                      next:        \n");   
    (void) gdt_ptr;
}  

void gdt_init(void) {
    gdt_set_descr(&gdt[0], 0, 0, 0, 0);                  // null entry 
    gdt_set_descr(&gdt[1], 0, 0xFFFFFFFF,   0x9A, 0xCF); // kernel code entry
    gdt_set_descr(&gdt[2], 0, 0xFFFFFFFF,   0x92, 0xCF); // kernel data entry
    gdt_set_descr(&gdt[3], 0, 0xFFFFFFFF,   0xFA, 0xCF); // user code entry
    gdt_set_descr(&gdt[4], 0, 0xFFFFFFFF,   0xF2, 0xCF); // user data entry

    gdt_ptr.limit = (sizeof(gdt_descr_t) * GDT_LEN) - 1;
    gdt_ptr.base  = (uintptr_t) &gdt;

    gdt_set(&gdt_ptr);
}
