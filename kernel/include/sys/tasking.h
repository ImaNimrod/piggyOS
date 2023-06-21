#ifndef _KERNEL_TASKING_H
#define _KERNEL_TASKING_H

#include <display.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stdint.h>
#include <stddef.h>

#define TASK_STATUS_ALIVE 0
#define TASK_STATUS_ZOMBIE 1
#define TASK_STATUS_DEAD 2

typedef uint32_t tid_t;

typedef struct context {
    uint32_t eax; // 0
    uint32_t ecx; // 4
    uint32_t edx; // 8
    uint32_t ebx; // 12
    uint32_t esp; // 16
    uint32_t ebp; // 20
    uint32_t esi; // 24
    uint32_t edi; // 28
    uint32_t eflags; //32
    uint32_t cr3; // 36
    uint32_t eip; //40
} context_t;

typedef struct task  {
    char name[128];    
    tid_t tid;  
    context_t* regs;
    page_directory_t* page_dir;
    struct task *prev, *next;
} task_t;

/* function declarations */
void tasking_init(void);
task_t* task_create(const char* name, uintptr_t location);

#endif
