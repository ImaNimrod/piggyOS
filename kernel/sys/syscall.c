#include <sys/syscall.h>

static int sys_hello(void) {
    kprintf("hello\n");
    return 0;
}

static int sys_goodbye(void) {
    kprintf("goodbye\n");
    return 0;
}

static uintptr_t syscall_table[] = {
    [SYS_HELLO]   = (uintptr_t) &sys_hello,
    [SYS_GOODBYE] = (uintptr_t) &sys_goodbye,
};

static void syscall_dispatcher(regs_t* r) {
    if (r->eax <= 0 && (NUM_ELEMENTS(syscall_table) - 1) && syscall_table[r->eax]) {
        klog(LOG_ERR, "Invalid syscall number: %d\n", r->eax);
        return;
    }

    uintptr_t location = syscall_table[r->eax];

    int ret;
    __asm__ volatile (" \
            push %1; \
            push %2; \
            push %3; \
            push %4; \
            push %5; \
            call *%6; \
            pop %%ebx; \
            pop %%ebx; \
            pop %%ebx; \
            pop %%ebx; \
            pop %%ebx; \
            " : "=a" (ret) : "r" (r->edi), "r" (r->esi), "r" (r->edx), "r" (r->ecx), "r" (r->ebx), "r" (location)
    );

    r->eax = ret;
}

void syscalls_init(void) {
    klog(LOG_OK, "Initializing syscall handler\n");
    int_install_handler(0x7f, syscall_dispatcher);
}
