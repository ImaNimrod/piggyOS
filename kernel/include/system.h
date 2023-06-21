#ifndef _KERNEL_SYSTEM_H
#define _KERNEL_SYSTEM_H

#include <stddef.h>
#include <stdint.h>

#define KERN_BASE 0xc0000000
#define USER_BASE 0x10000000

#define ARG_MAX 1024

#define K 1024
#define M (K * K)
#define G (K * M)

// version name / number
#define VERSION_MAJ 1
#define VERSION_MIN 0
#define VERSION_ALIAS "kerplunk"

#define ALIGN_UP(val, a) (((val) + ((a) - 1)) & ~((a) - 1))
#define ALIGN_DOWN(val, a) ((val) & ~((a) - 1))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define NUM_ELEMENTS(array) (sizeof(array) / sizeof(array[0]))

typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;    
} regs_t;

static uint32_t align_to(uint32_t n, uint32_t align) {
    if (n % align == 0) {
        return n;
    }

    return n + (align - n % align);
}

static uint32_t divide_up(uint32_t n, uint32_t d) {
    if (n % d == 0)
        return n / d;

    return 1 + n / d;
}

/* defined by the linker */
extern uintptr_t kernel_start_phys;
extern uintptr_t kernel_start_virt;
extern uintptr_t kernel_end_phys;
extern uintptr_t kernel_end_virt;

#endif
