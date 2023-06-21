#include <drivers/fpu.h>

static inline void fpu_load_control_word(uint16_t control) {
    __asm__ volatile("fldcw %0;"::"m"(control)); 
}

static void fpu_exception_handler(regs_t* r) {
    __asm__ volatile("clts");
    kprintf("FPU EXCEPTION\n");
    (void) r;
}

void fpu_init(void) {
	klog(LOG_OK, "Enabling floating-point unit\n");

    if (!check_edx_feature(CPUID_FEAT_EDX_FPU)) {
        klog(LOG_ERR, "builtin FPU not present\n");
        return;
    }

    int_install_handler(7, &fpu_exception_handler);
    __asm__ volatile("fninit");

    uint32_t cr0;
	__asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x018;
	__asm__ volatile ("mov %0, %%cr0" :: "r"(cr0));

	uint32_t cr4;
	__asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= 0x200;
	__asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));
}
