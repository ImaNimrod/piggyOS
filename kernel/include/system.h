#ifndef _KERNEL_SYSTEM_H
#define _KERNEL_SYSTEM_H

#include <stdint.h>

// litteraly a bunch of commonly used constants
#define LOAD_MEMORY_ADDRESS 0xC0000000

#define K 1024
#define M (1024*K)
#define G (1024*M)

// version name / number
#define VERSION_MAJ 1
#define VERSION_MIN 0
#define VERSION_ALIAS "kerplunk"

// defined by the linker
extern uintptr_t kernel_start;
extern uintptr_t kernel_end;

typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;    
} regs_t;

#endif
