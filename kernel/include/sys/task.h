#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <display.h>
#include <dsa/list.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <stdint.h>
#include <system.h>
#include <sys/spinlock.h>
#include <types.h>

typedef struct{
	uint32_t ebx, esi, edi, ebp;  // callee-save
	uint32_t eip, eflags;	        // CALL/IRET
} context_t;

typedef struct {
    uint8_t* kstack;
	uint8_t* kstack_mem;
    page_directory_t* page_dir;
    char name[16];
    pid_t pid;
    list_node_t* self;
} task_t;

extern void context_switch(task_t* t);

/* function declarations */
void task_create(char* name, uintptr_t func);
void schedule(void);
void multitasking_init(char* name, uintptr_t func);

#endif
