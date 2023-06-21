#include <sys/syscall.h>

extern task_t* current_task;

static int sys_rpl3test(void) {
    kprintf("hello from usermode (rpl=3)\n");
    return 0;
}

static uintptr_t syscall_table[] = {
    [SYS_GETTID]  = (uintptr_t) &sys_gettid,
    [SYS_YIELD]   = (uintptr_t) &sys_yield,
    [SYS_EXIT]    = (uintptr_t) &sys_exit,
    [SYS_RPL3TEST] = (uintptr_t) &sys_rpl3test,
};

static void syscall_dispatcher(regs_t* r) {
    (void) r;

    regs_t* ifr = current_task->ifr;

    if (ifr->eax <= 0 || ifr->eax > (NUM_ELEMENTS(syscall_table) - 1)) {
        klog(LOG_ERR, "Invalid syscall number: %d\n", ifr->eax);
        ifr->eax = -1;
        return;
    }

    uintptr_t location = syscall_table[ifr->eax];
    if (!location) {
        klog(LOG_ERR, "Invalid syscall number: %d\n", ifr->eax);
        ifr->eax = -1;
        return;
    }

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
            " : "=a" (ret) : "r" (ifr->edi), "r" (ifr->esi), "r" (ifr->edx), "r" (ifr->ecx), "r" (ifr->ebx), "r" (location)
    );

    ifr->eax = ret;
}

void syscalls_init(void) {
    klog(LOG_OK, "Initializing syscall handler\n");
    int_install_handler(0x7f, &syscall_dispatcher);
}
