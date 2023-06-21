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
		kernel_text_start = .;
		*(.multiboot2_header)
		*(.text .text.*)
		kernel_text_end = .;
	}

    .data ALIGN(0x1000) : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE)
	{
		kernel_data_start = .;
		*(.data .data.*)
		kernel_data_end = .;
	}

	.bss ALIGN(0x1000) : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE)
	{
		*(COMMON)
		*(.bss .bss.*)
		*(.stack)
	}

    kernel_end = .;

    /DISCARD/ :
	{
		*(.comment)
        *(.eh_frame)
		*(.note.gnu.build-id)
	}
}
