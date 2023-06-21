OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS {
    . = 0x10000000;

    .text ALIGN(0x1000) : AT(ADDR(.text)) {
        *(.text)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data)) {
        *(.data)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata)) {
        *(.rodata*)
    }

	.bss ALIGN(0x1000) : AT(ADDR(.bss)) {
        *(COMMON)
        *(.bss)
    }
}
