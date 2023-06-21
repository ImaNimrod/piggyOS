OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(start)

SECTIONS {
	. = 1M; 

    KERNEL_VIRTUAL_BASE = 0xc0000000;
    . += KERNEL_VIRTUAL_BASE;

	kernel_start = .;

    .text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_VIRTUAL_BASE)
	{
		*(.multiboot2_header)
		*(.text)
        *(.rodata*)
	}

    .data ALIGN(0x1000) : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE)
	{
		*(.data)
	}

	.bss ALIGN(0x1000) : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE)
	{
		*(COMMON)
		*(.bss)
	}

    kernel_end = .;

    /DISCARD/ :
	{
		*(.comment)
        *(.eh_frame)
		*(.note.gnu.build-id)
	}
}
