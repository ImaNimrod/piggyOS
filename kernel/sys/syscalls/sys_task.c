#include <sys/syscall.h>

// syscalls dealing with multitasking

extern task_t* current_task;

int sys_gettid(void) {
    return current_task->tid;
}

int sys_yield(void) {
    schedule();
    return 0;
}

int sys_exit(void) {
    task_exit();
    return 0; // we never reach this
}
