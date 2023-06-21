#include <drivers/fpu.h>

extern task_t* current_task;
static task_t* last_fpu_task;

static void fpu_exception_handler(regs_t* r) {
    (void) r;

    __asm__ volatile("clts");

    if (last_fpu_task != NULL) {
        __asm__ volatile("fxsave %0"
                         : "=m"(*current_task->fpu_state) : : "memory");
    } 

    if (!current_task->fpu_used) {
        __asm__ volatile("fninit");
        __asm__ volatile("fxsave %0"
					     : "=m"(*current_task->fpu_state) : : "memory");

		current_task->fpu_used = 1;
    } else {
		__asm__ volatile("fxrstor %0" 
                         : : "m"(*current_task->fpu_state));
    }

    last_fpu_task = current_task;
}

void fpu_init(void) {
	klog(LOG_OK, "Enabling floating-point unit\n");

    if (!check_edx_feature(CPUID_FEAT_EDX_FPU)) {
        klog(LOG_ERR, "Builtin FPU not present\n");
        return;
    }

    int_install_handler(7, &fpu_exception_handler);
    __asm__ volatile("fninit");

    /* enable FPU native exceptions and task switched bits */
    uint32_t cr0;
	__asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x018;
	__asm__ volatile ("mov %0, %%cr0" :: "r"(cr0));

    /* set OSFXSR bit to enable saving the SSE state */ 
	uint32_t cr4;
	__asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= 0x200;
	__asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));
}
