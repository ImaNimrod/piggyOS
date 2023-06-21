#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <cpu/desc_tbls.h>
#include <display.h>
#include <fs/fs.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <stdint.h>
#include <system.h>
#include <types.h>

enum {
    TASK_RUNNING  = 1,
    TASK_SLEEPING = 2,
    TASK_ZOMBIE   = 3,
};

typedef struct {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
} context_t;

typedef struct {
    pid_t pid;
    int state;
    list_node_t* self;
    context_t* context;
    regs_t* isr_frame;
    page_directory_t* page_dir;
} task_t;

typedef void (*task_entry_t) (void);

// defined in cpu/tss.c
extern tss_entry_t tss;

// defined in memory/vmm.c 
extern page_directory_t* kernel_page_dir;

/* function declarations */
void schedule(void);
void tasking_init(void);
task_t* task_create(task_entry_t entry);
void task_delete(task_t* t);

extern void context_switch(context_t** old_ctx, context_t* new_ctx);

#endif
