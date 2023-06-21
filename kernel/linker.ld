ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
   . = 0x00100000;

   low_kernel_start = .;

   .multiboot2_header :  { *(.multiboot2_header) }

   .low_text ALIGN (0x1000) : { *(.low_text) }
   .low_data ALIGN (0x1000) : { *(.low_data) }
   .low_bss ALIGN (0x1000) :
   {
       *(.low_bss)
   }

   low_kernel_end = .;

   . += 0xC0000000;

   kernel_start = .;

   .text ALIGN (0x1000) : AT(ADDR(.text) - 0xC0000000) { *(.text) }
   .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xC0000000) { *(.rodata*) }
   .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000) { *(.data) }
   .bss ALIGN (0x1000) : AT(ADDR(.bss) - 0xC0000000)
   {
       *(COMMON)
       *(.bss)
   }

   kernel_end = .;

   kernel_size = kernel_end - kernel_start;
}

ASSERT(kernel_size < 0x400000, "kernel exceeds the 4 MB limit. entry.asm must be updated to work with a >3MB kernel");
