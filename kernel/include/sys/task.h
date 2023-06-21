#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <cpu/desc_tbls.h>
#include <display.h>
#include <dsa/list.h>
#include <fs/fs.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <stdint.h>
#include <system.h>
#include <sys/spinlock.h>
#include <types.h>

#define MAX_FILEDESCS 16

typedef struct {
    fs_node_t* node;
    off_t offset; 
} task_filedesc_t;

typedef struct {
	uint32_t ebx, esi, edi, ebp;  // callee-save
	uint32_t eip, eflags;	        // CALL/IRET
} context_t;

enum state { 
    TASK_READY, 
    TASK_RUNNING, 
    TASK_DEAD,
};

typedef struct {
    uint8_t* kstack;
	uint8_t* kstack_mem;
    page_directory_t* page_dir;
    list_node_t* self;

    char name[16];
    pid_t pid;
    enum state state;

    task_filedesc_t file_descs[MAX_FILEDESCS];
    uint32_t file_count;
} task_t;

extern void context_switch(task_t* t);

/* function declarations */
void task_create(char* name, uintptr_t func);
void task_exit(int code);
void schedule(void);
void multitasking_init(char* name, uintptr_t func);

#endif
