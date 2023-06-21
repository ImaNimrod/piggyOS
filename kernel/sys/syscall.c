#include <sys/syscall.h>

uint32_t num_syscalls;

static int sys_gettid(void) {
    kprintf("hello");
    return 0;
}

static uintptr_t syscall_table[] = {
    (uintptr_t) &sys_gettid,
    0,
};

static void syscall_dispatcher(regs_t* r) {
    if(r->eax >= num_syscalls) {
        klog(LOG_ERR, "Invalid syscall number: %d\n", r->eax);
        return;
    }

    memcpy(&saved_context, r, sizeof(regs_t));
    uintptr_t location = syscall_table[r->eax];

    int32_t ret;
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
    for(num_syscalls = 0; syscall_table[num_syscalls]; num_syscalls++);
    int_install_handler(0x7f, syscall_dispatcher);
}
