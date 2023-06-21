#include <cpu/cpuid.h>

cpuid_info_t cpuid_features;

const char *Intel[] = {
    "Brand ID Not Supported.", 
    "Intel(R) Celeron(R) processor", 
    "Intel(R) Pentium(R) III processor", 
    "Intel(R) Pentium(R) III Xeon(R) processor", 
    "Intel(R) Pentium(R) III processor", 
    "Reserved", 
    "Mobile Intel(R) Pentium(R) III processor-M", 
    "Mobile Intel(R) Celeron(R) processor", 
    "Intel(R) Pentium(R) 4 processor", 
    "Intel(R) Pentium(R) 4 processor", 
    "Intel(R) Celeron(R) processor", 
    "Intel(R) Xeon(R) Processor", 
    "Intel(R) Xeon(R) processor MP", 
    "Reserved", 
    "Mobile Intel(R) Pentium(R) 4 processor-M", 
    "Mobile Intel(R) Pentium(R) Celeron(R) processor", 
    "Reserved", 
    "Mobile Genuine Intel(R) processor", 
    "Intel(R) Celeron(R) M processor", 
    "Mobile Intel(R) Celeron(R) processor", 
    "Intel(R) Celeron(R) processor", 
    "Mobile Geniune Intel(R) processor", 
    "Intel(R) Pentium(R) M processor", 
    "Mobile Intel(R) Celeron(R) processor"
};

const char *Intel_Other[] = {
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Intel(R) Celeron(R) processor", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Intel(R) Xeon(R) processor MP", 
    "Reserved", 
    "Reserved", 
    "Intel(R) Xeon(R) processor", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved"
};

static long has_cpuid(void) {
    long has_cpuid = 0;

    __asm__ volatile(
    "        pushfl                      \n"
    "        popl    %%eax               \n"
    "        movl    %%eax,%%ecx         \n"
    "        xorl    $0x200000,%%eax     \n"
    "        pushl   %%eax               \n"
    "        popfl                       \n"
    "        pushfl                      \n"
    "        popl    %%eax               \n"
    "        xorl    %%ecx,%%eax         \n"
    "        jz      1f                  \n"
    "        movl    $1,%[has_cpuid]     \n"
    "1:                                                                    \n"
    : [has_cpuid]"=m" (has_cpuid)
    :
    : "%rax", "%rcx"
    );

    return has_cpuid;
}

static void cpuid(uint32_t req, cpuid_t *cpuid_info) {
    __asm__ volatile(
        "cpuid"
        : "=a" (cpuid_info->eax),
          "=b" (cpuid_info->ebx),
          "=c" (cpuid_info->ecx),
          "=d" (cpuid_info->edx)
        : "a" (req));
}

static int do_intel(void) {
    kprintf("Intel specific Features:\n");
    
    cpuid_t cpuid_info;
    uint32_t eax, ebx, max_eax, signature;
    int model, family, type, brand, stepping, reserved;
    int extended_family = -1;
    
    cpuid(CPUID_GETFEATURES, &cpuid_info);

    eax = cpuid_info.eax;
    ebx = cpuid_info.ebx;
    
    model = (eax >> 4) & 0xf;
    family = (eax >> 8) & 0xf;
    type = (eax >> 12) & 0x3;
    brand = ebx & 0xff;
    stepping = eax & 0xf;
    reserved = eax >> 14;
    signature = eax;

    cpuid_features.model = model;
    cpuid_features.family = family;
    cpuid_features.type = type;
    cpuid_features.brand = brand;    
    cpuid_features.step = stepping;
    cpuid_features.fill = reserved;
    cpuid_features.cpuid = signature;
        
    kprintf("Type %d - ", type);
    switch(type) {
        case 0:
            kprintf("Original OEM");
            break;
        case 1:
            kprintf("Overdrive");
            break;
        case 2:
            kprintf("Dual-capable");
            break;
        case 3:
            kprintf("Reserved");
            break;
    }

    kprintf("\n");
    kprintf("Family %d - ", family);
    switch(family) {
        case 3:
            kprintf("i386");
            break;
        case 4:
            kprintf("i486");
            break;
        case 5:
            kprintf("Pentium");
            break;
        case 6:
            kprintf("Pentium Pro");
            break;
        case 15:
            kprintf("Pentium 4");
            break;
        default:
            kprintf("Unknown CPU family");
    }
    kprintf("\n");
    
    if (family == 15) {
        extended_family = (eax >> 20) & 0xff;
        kprintf("Extended family %d\n", extended_family);
    }
    
    kprintf("Model %d - ", model);
    switch(family) {
        case 3:
            break;
        case 4:
            switch(model) {
            case 0: break;
            case 1: 
                kprintf("DX");
                break;
            case 2:
                kprintf("SX");
                break;
            case 3:
                kprintf("487/DX2");
                break;
            case 4:
                kprintf("SL");
                break;
            case 5:
                kprintf("SX2");
                break;
            case 7:
                kprintf("Write-back enhanced DX2");
            break;
            case 8:
                kprintf("DX4");
                break;
            default:
                kprintf("Unknown CPU model");
            }
            break;
        case 5:
            switch(model) {
                case 1:
                    kprintf("60/66");
                    break;
                case 2:
                    kprintf("75-200");
                    break;
                case 3:
                    kprintf("for 486 system");
                    break;
                case 4:
                    kprintf("MMX");
                    break;
                default:
                    kprintf("Unknown CPU model");
            }
            break;
        case 6:
            switch(model) {
                case 1:
                    kprintf("Pentium Pro");
                    break;
                case 3:
                    kprintf("Pentium II Model 3");
                    break;
                case 5:
                    kprintf("Pentium II Model 5/Xeon/Celeron");
                    break;
                case 6:
                    kprintf("Celeron");
                    break;
                case 7:
                    kprintf("Pentium III/Pentium III Xeon - external L2 cache");
                    break;
                case 8:
                    kprintf("Pentium III/Pentium III Xeon - internal L2 cache");
                    break;
                default:
                    kprintf("Unknown CPU model");
                }
            break;
        case 15:
            break;
    }
    kprintf("\n");
    
    cpuid(CPUID_INTELEXTENDED, &cpuid_info);
    max_eax = cpuid_info.eax;
      
    if (max_eax >= 0x80000004) {
        kprintf("Brand: ");
        if(max_eax >= 0x80000002) /* CPUID_INTELBRANDSTRING */
            cpuid(CPUID_INTELBRANDSTRING, &cpuid_info);
        if(max_eax >= 0x80000003) /* CPUID_INTELBRANDSTRINGMORE */
            cpuid(CPUID_INTELBRANDSTRINGMORE, &cpuid_info);
        if (max_eax >= 0x80000004) /* CPUID_INTELBRANDSTRINGEND */
            cpuid(CPUID_INTELBRANDSTRINGEND, &cpuid_info);
        kprintf("\n");
    } else if(brand > 0) {
        kprintf("Brand %d - ", brand);
        if (brand < 0x18) {
            if(signature == 0x000006b1 || signature == 0x00000f13) {
                kprintf("%s\n", Intel_Other[brand]);
            } else {
                kprintf("%s\n", Intel[brand]);
            }
        } else {
            kprintf("Reserved\n");
        }
    }

    kprintf("Stepping: %d Reserved: %d\n", stepping, reserved);
    return 0;
}

int do_amd(void) {
    kprintf("AMD specific features:\n");
    cpuid_t cpuid_info;
    uint32_t extended, eax;
    uint32_t family, model, stepping, reserved;
    
    cpuid(CPUID_GETFEATURES, &cpuid_info);

    eax = cpuid_info.eax;

    model = (eax >> 4) & 0xf;
    family = (eax >> 8) & 0xf;
    stepping = eax & 0xf;
    reserved = eax >> 12;

    kprintf("Family: %d Model: %d [", family, model);
    switch(family) {
        case 4:
            kprintf("486 Model %d", model);
            break;
        case 5:
            switch(model) {
                case 0: break;
                case 1: break; 
                case 2: break;
                case 3: break;
                case 6: break;
                case 7:
                    kprintf("K6 Model %d", model);
                    break;
                case 8:
                    kprintf("K6-2 Model 8");
                    break;
                case 9:
                    kprintf("K6-III Model 9");
                    break;
                default:
                    kprintf("K5/K6 Model %d", model);
                    break;
            }
            break;
        case 6:
        switch(model) {
            case 1: break;
            case 2: break;
            case 3:
                kprintf("Duron Model 3");
                break;
            case 4:
                kprintf("Athlon Model %d", model);
                break;
            case 6:
                kprintf("Athlon MP/Mobile Athlon Model 6");
                break;
            case 7:
                kprintf("Mobile Duron Model 7");
                break;
            default:
                kprintf("Duron/Athlon Model %d", model);
                break;
        }
        break;
    }
    kprintf("]\n");
    
    cpuid(CPUID_INTELEXTENDED,&cpuid_info);
    extended = cpuid_info.eax; 
    
    if (extended == 0) 
        return 0;

    if(extended >= 0x80000002) {
        uint32_t j;
        kprintf("Detected Processor Name: ");

        for (j = 0x80000002; j <= 0x80000004; j++)
            cpuid(j, &cpuid_info);
        kprintf("\n");
    }
    if(extended >= 0x80000007) {
        cpuid(0x80000007, &cpuid_info);
        if(cpuid_info.edx & 1) 
            kprintf("Temperature Sensing Diode Detected!\n");
    }

    kprintf("Stepping: %d Reserved: %d\n", stepping, reserved);
    return 0;
}

int detect_cpu(void) {
    cpuid_t cpuid_info;

    klog(LOG_OK, "Identifying CPU type\n");
    if (!has_cpuid()) {
        klog(LOG_WARN, "CPU does not support cpuid instrcution\n");
        return 1;
    }

    cpuid(CPUID_GETVENDORSTRING, &cpuid_info);
    switch(cpuid_info.ebx)  {
        case 0x756e6547: /* Intel magic code */
            kprintf("Intel CPU detected\n");
            do_intel();
            break;
        case 0x68747541: /* AMD magic code */
            kprintf("AMD CPU detected\n");
            do_amd();
            break;
        default:
            kprintf("unknown x86 CPU detected\n");
            break;
    }

    return 0;
}
