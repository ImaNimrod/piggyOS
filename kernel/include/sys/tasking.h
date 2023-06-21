#ifndef _KERNEL_TASKING_H
#define _KERNEL_TASKING_H

#include <display.h>
#include <dsa/list.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stdint.h>
#include <stddef.h>

#define TASK_STATE_ALIVE 0
#define TASK_STATE_ZOMBIE 1
#define TASK_STATE_DEAD 2

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

typedef struct {
    char* name;    
    tid_t tid;  
    context_t regs;
    uint32_t state;
    page_directory_t* page_dir;
    list_node_t* self;
} task_t;

/* defined in vmm.h */
extern page_directory_t* kernel_page_dir;

/* usermode.asm */
extern void user_regs_switch(context_t* r);

/* function declarations */
void tasking_init(void);
void task_schedule(void);
void create_task(char* name, uintptr_t location);

#endif
