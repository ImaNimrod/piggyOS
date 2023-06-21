#ifndef _KERNEL_SYSTEM_H
#define _KERNEL_SYSTEM_H

#include <stdint.h>

// virtual 
#define LOAD_MEMORY_ADDRESS 0xC0000000

#define K 1024
#define M (1024*K)
#define G (1024*M)

// version name / number
#define VERSION_MAJ 1
#define VERSION_MIN 0
#define VERSION_ALIAS "kerplunk"

#define ALIGN_UP(val, a) (((val) + ((a) - 1)) & ~((a) - 1))
#define ALIGN_DOWN(val, a) ((val) & ~((a) - 1))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define NUM_ELEMENTS(array) (sizeof(array) / sizeof(array[0]))

static uint32_t divide_up(uint32_t n, uint32_t d) {
    if (n % d == 0)
        return n / d;

    return 1 + n / d;
}

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
