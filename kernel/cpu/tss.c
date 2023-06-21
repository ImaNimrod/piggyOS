#include <cpu/desc_tbls.h>

tss_entry_t tss;

static void tss_flush(uint16_t index) {
    __asm__ volatile("ltr %w0" : :  "q" (index * 8));
    return;
}

void tss_init(uint32_t index, uint32_t kss, uint32_t kesp) {
    uint32_t base = (uint32_t) &tss;
    gdt_set_descr(index, base, base + sizeof(tss_entry_t), 0xE9, 0);

    memset(&tss, 0, sizeof(tss_entry_t));

    tss.ss0 = kss;
    tss.esp0 = kesp;
    tss.cs = 0x0b;
    tss.ds = 0x13;
    tss.es = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;
    tss.ss = 0x13;

    tss_flush(index);
}

void tss_set_stack(uint32_t kss, uint32_t kesp) {
    tss.ss0 = kss;
    tss.esp0 = kesp;
}
