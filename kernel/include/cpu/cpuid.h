#ifndef _KERNEL_CPUID_H
#define _KERNEL_CPUID_H

#include <display.h>
#include <stdint.h>

#define CPUID_GETVENDORSTRING       0
#define CPUID_GETFEATURES           1
#define CPUID_GETTLB                2
#define CPUID_GETSERIAL             3
#define CPUID_INTELEXTENDED         0x80000000
#define CPUID_INTELFEATURES         0x80000001
#define CPUID_INTELBRANDSTRING      0x80000002
#define CPUID_INTELBRANDSTRINGMORE  0x80000003
#define CPUID_INTELBRANDSTRINGEND   0x80000004

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} cpuid_t;

typedef struct {
    char        type;
    char        model;
    char        family;
    char        brand;
    char        step;
    char        fill;
    uint32_t    cpuid;
    uint32_t    capability;
    char        vend_id[12];
    uint8_t     cache_info[16];
    uint32_t    pwrcap;
    uint32_t    ext;
    uint32_t    feature_flag;
    uint32_t    dcache0_eax;
    uint32_t    dcache0_ebx;
    uint32_t    dcache0_ecx;
    uint32_t    dcache0_edx;
    uint32_t    dcache1_eax;
    uint32_t    dcache1_ebx;
    uint32_t    dcache1_ecx;
    uint32_t    dcache1_edx;    
    uint32_t    dcache2_eax;
    uint32_t    dcache2_ebx;
    uint32_t    dcache2_ecx;
    uint32_t    dcache2_edx;    
    uint32_t    dcache3_eax;
    uint32_t    dcache3_ebx;
    uint32_t    dcache3_ecx;
    uint32_t    dcache3_edx;
} cpuid_info_t;

/* function declarations */
int detect_cpu(void);

#endif
