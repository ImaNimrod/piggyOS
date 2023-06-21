#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <cpu/fpu.h>
#include <display.h>
#include <drivers/timer.h>
#include <dsa/list.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <stdint.h>
#include <system.h>
#include <types.h>

#define KSTACK_SIZE 0x1000
#define MAX_FILEDESCS 16

struct context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
    uint32_t eflags;
};

enum task_state { 
    TASK_READY, 
    TASK_SLEEPING,
    TASK_ZOMBIE,
};

typedef struct {
    void* kstack;
	void* kstack_mem;
    page_directory_t* page_dir;
    regs_t* ifr;

    __attribute__((aligned(16))) uint8_t* fpu_state; // space for fpu/sse/mmx registers 
    uint8_t fpu_used;   // have we used the fpu yet

    list_node_t* self;

    tid_t tid;
    enum task_state state;
    int exit_code;
    uint32_t usage;
} task_t;

/* function declarations */
task_t* task_create(uintptr_t func);
void task_destroy(task_t* t);
void task_exit(int code);
void schedule(void);
void multitasking_init(uintptr_t func);

extern void context_switch(task_t* next);

#endif
