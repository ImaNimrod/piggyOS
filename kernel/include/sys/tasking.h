#ifndef _KERNEL_TASKING_H
#define _KERNEL_TASKING_H

#include <cpu/desc_tbls.h>
#include <display.h>
#include <dsa/list.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stdbool.h>
#include <stdint.h>
#include <system.h>

#define TASK_STATE_ALIVE 0
#define TASK_STATE_ZOMBIE 1
#define TASK_STATE_DEAD 2

typedef uint16_t tid_t;

typedef struct {
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

typedef struct task {
	char name[128];
	tid_t tid;
    context_t regs;
    node_t* self;
    void* stack;
    page_directory_t* page_dir;
    uint32_t state;
} task_t;

extern page_directory_t* kernel_page_dir;
extern tss_entry_t tss;

/* defined in usermode.asm */
extern void user_regs_switch(context_t* regs2);
extern void kernel_regs_switch(context_t* regs2);

void tasking_init(void);
void create_task(const char* name, void *loc);
tid_t get_tid(void);

#endif

