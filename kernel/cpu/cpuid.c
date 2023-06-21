#include <cpu/cpuid.h>

int check_ecx_feature(uint32_t feature) {
    unsigned int eax, unused, ecx;
    __get_cpuid(1, &eax, &unused, &ecx, &unused);
    return ecx & feature;
}

int check_edx_feature(uint32_t feature) {
    unsigned int unused, edx;
    __get_cpuid(1, &unused, &unused, &unused, &edx);
    return edx & feature;
}

void print_cpu_vendor(void) {
    unsigned int unused;
    uint32_t vendor_string[4] = {0};
    __cpuid(0, unused, vendor_string[0], vendor_string[2], vendor_string[1]);

    kprintf("CPU vendor: %s\n", vendor_string);
}

const char* edx_feature_flags[32] = {
    "fpu",        // CPUID_FEAT_EDX_FPU    
    "vme",        // CPUID_FEAT_EDX_VME    
    "de",         // CPUID_FEAT_EDX_DE     
    "pse",        // CPUID_FEAT_EDX_PSE    
    "tsc",        // CPUID_FEAT_EDX_TSC    
    "msr",        // CPUID_FEAT_EDX_MSR    
    "pae",        // CPUID_FEAT_EDX_PAE    
    "mce",        // CPUID_FEAT_EDX_MCE    
    "cx8",        // CPUID_FEAT_EDX_CX8    
    "apic",       // CPUID_FEAT_EDX_APIC   
    "sep",        // CPUID_FEAT_EDX_SEP    
    "mtrr",       // CPUID_FEAT_EDX_MTRR   
    "pge",        // CPUID_FEAT_EDX_PGE    
    "mca",        // CPUID_FEAT_EDX_MCA    
    "cmov",       // CPUID_FEAT_EDX_CMOV   
    "pat",        // CPUID_FEAT_EDX_PAT    
    "pse36",      // CPUID_FEAT_EDX_PSE36  
    "psn",        // CPUID_FEAT_EDX_PSN    
    "clflush",    // CPUID_FEAT_EDX_CLFLUSH
    "ds",         // CPUID_FEAT_EDX_DS     
    "acpi",       // CPUID_FEAT_EDX_ACPI   
    "mmx",        // CPUID_FEAT_EDX_MMX    
    "fxsr",       // CPUID_FEAT_EDX_FXSR   
    "sse",        // CPUID_FEAT_EDX_SSE    
    "sse2",       // CPUID_FEAT_EDX_SSE2   
    "ss",         // CPUID_FEAT_EDX_SS     
    "htt",        // CPUID_FEAT_EDX_HTT    
    "tm",         // CPUID_FEAT_EDX_TM     
    "ia64",       // CPUID_FEAT_EDX_IA64   
    "pbe",        // CPUID_FEAT_EDX_PBE    
};

const char* ecx_feature_flags[32] = {
    "sse3",       // CPUID_FEAT_ECX_SSE3       
    "pclmul",     // CPUID_FEAT_ECX_PCLMUL     
    "dtes64",     // CPUID_FEAT_ECX_DTES64     
    "monitor",    // CPUID_FEAT_ECX_MONITOR    
    "ds_cpl",     // CPUID_FEAT_ECX_DS_CPL     
    "vmx",        // CPUID_FEAT_ECX_VMX        
    "smx",        // CPUID_FEAT_ECX_SMX        
    "est",        // CPUID_FEAT_ECX_EST        
    "tm2",        // CPUID_FEAT_ECX_TM2        
    "ssse3",      // CPUID_FEAT_ECX_SSSE3      
    "cid",        // CPUID_FEAT_ECX_CID        
    "sdbg",       // CPUID_FEAT_ECX_SDBG       
    "fma",        // CPUID_FEAT_ECX_FMA        
    "cx16",       // CPUID_FEAT_ECX_CX16       
    "xtpr",       // CPUID_FEAT_ECX_XTPR       
    "pdcm",       // CPUID_FEAT_ECX_PDCM       
    "pcid",       // CPUID_FEAT_ECX_PCID       
    "dca",        // CPUID_FEAT_ECX_DCA        
    "sse4_1",     // CPUID_FEAT_ECX_SSE4_1     
    "sse4_2",     // CPUID_FEAT_ECX_SSE4_2     
    "x2apic",     // CPUID_FEAT_ECX_X2APIC     
    "movbe",      // CPUID_FEAT_ECX_MOVBE      
    "popcnt",     // CPUID_FEAT_ECX_POPCNT     
    "tsc",        // CPUID_FEAT_ECX_TSC        
    "aes",        // CPUID_FEAT_ECX_AES        
    "xsave",      // CPUID_FEAT_ECX_XSAVE      
    "osxsave",    // CPUID_FEAT_ECX_OSXSAVE    
    "avx",        // CPUID_FEAT_ECX_AVX        
    "f16c",       // CPUID_FEAT_ECX_F16C       
    "rdrand",     // CPUID_FEAT_ECX_RDRAND     
    "hypervisor", // CPUID_FEAT_ECX_HYPERVISOR 
};

void print_cpu_features(void) {
    unsigned int unused, ecx, edx;
    __get_cpuid(1, &unused, &unused, &ecx, &edx);

    int i;

    kprintf("CPU feature flags: ");
    for (i = 0; i < 31; i++) {
        if (edx & (1 << i)) {
            kprintf("%s ", edx_feature_flags[i]);
        }
    }

    for (i = 0; i < 31; i++) {
        if (ecx & (1 << i)) {
            kprintf("%s ", ecx_feature_flags[i]);
        }
    }

    kprintf("\n");
}
