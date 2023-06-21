#ifndef _KERNEL_SYSTEM_H
#define _KERNEL_SYSTEM_H

#include <display.h>
#include <stdint.h>

// litteraly a bunch of commonly used constants
#define LOAD_MEMORY_ADDRESS 0xC0000000

#define K 1024
#define M (1024*K)
#define G (1024*M)

#define assert(statement) ((statement) ? (void)0 : assert_failed(__FILE__, __LINE__, #statement))
void assert_failed(const char *file, uint32_t line, const char *desc);

// defined by the linker
extern uintptr_t kernel_start;
extern uintptr_t kernel_end;

typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;    
} regs_t;

/* function declarations */
static inline void enable_int(void) {
    __asm__ volatile("sti");
}

static inline void disable_int(void) {
    __asm__ volatile("cli");
}


#endif
