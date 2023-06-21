#include <drivers/fpu.h>

void fpu_set_cw(uint16_t cw) {
	__asm__ volatile("fldcw %0" :: "m"(cw));
}

void fpu_init(void) {
	klog(LOG_OK, "Enabling floating-point unit\n");

	uint32_t cr4;
	__asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= 0x200;
	__asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));

	fpu_set_cw(0x37f);
    fpu_set_cw(0x37e);
    fpu_set_cw(0x37a);
}
