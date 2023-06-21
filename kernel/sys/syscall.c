#include <sys/syscall.h>

uint32_t num_syscalls;

static int sys_gettid(void) {
    return (int) get_tid;
}

static uintptr_t syscall_table[] = {
   (uintptr_t) &sys_gettid,
};

static void syscall_dispatcher(regs_t* r) {
    if (r->eax >= num_syscalls && syscall_table[r->eax]) {
        uintptr_t ptr = syscall_table[r->eax];

        memcpy(&saved_context, r, sizeof(regs_t));

        uint32_t ret;
        __asm__ volatile (
            "push %1\n"
            "push %2\n"
            "push %3\n"
            "push %4\n"
            "push %5\n"
            "call *%6\n"
            "pop %%ebx\n"
            "pop %%ebx\n"
            "pop %%ebx\n"
            "pop %%ebx\n"
            "pop %%ebx\n"
            : "=a" (ret) : "r" (r->edi), "r" (r->esi), "r" (r->edx), "r" (r->ecx), "r" (r->ebx), "r" (ptr)
        );

        r->eax = ret;
    } 
    klog(LOG_ERR, "Invalid syscall number: %d\n", r->eax);
}

void syscalls_init(void) {
    klog(LOG_OK, "Initializing syscall handler\n");
    for(num_syscalls = 0; syscall_table[num_syscalls] != 0; num_syscalls++);
    isrs_install_handler(0x7f, syscall_dispatcher);
}
