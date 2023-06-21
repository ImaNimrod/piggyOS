#ifndef _KERNEL_SYSTEM_H
#define _KERNEL_SYSTEM_H

#include <stdint.h>

// litteraly a bunch of commonly used constants
#define LOAD_MEMORY_ADDRESS 0xC0000000

#define K 1024
#define M (1024*K)
#define G (1024*M)

// defined by the linker
extern uintptr_t kernel_start;
extern uintptr_t kernel_end;

typedef struct {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    
} regs_t;

#endif
