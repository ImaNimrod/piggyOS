#ifndef _KERNEL_PROCESS_H
#define _KERNEL_PROCESS_H

#include <display.h>
#include <fs/fs.h>
#include <memory/kheap.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <stdint.h>

#define NUM_PROC 64

typedef int pid_t;

typedef struct {
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
} context_t;

enum process_state { 
    UNUSED, 
    EMBRYO, 
    SLEEPING, 
    RUNNABLE, 
    RUNNING, 
    ZOMBIE 
};

typedef struct {
    char name[16];
    pid_t pid;
    page_directory_t* page_dir;
    context_t* context;
} process_t;

/* function declarations */
extern void context_switch(context_t** old, context_t* new);

#endif
