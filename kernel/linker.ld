OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(start)

SECTIONS {
	. = 1M; 

    KERNEL_VIRTUAL_BASE = 0xc0000000;
    . += KERNEL_VIRTUAL_BASE;

	kernel_start_phys = . - KERNEL_VIRTUAL_BASE;
	kernel_start_virt = .;

    .multiboot2_header ALIGN(8) : AT(ADDR(.multiboot2_header) - KERNEL_VIRTUAL_BASE) {
        *(.multiboot2_header)
    }

    .text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_VIRTUAL_BASE) {
		*(.text)
        *(.rodata*)
	}

    .data ALIGN(0x1000) : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE) {
		*(.data)
	}

	.bss ALIGN(0x1000) : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE) {
		*(COMMON)
		*(.bss)
	}

	kernel_end_phys = . - KERNEL_VIRTUAL_BASE;
    kernel_end_virt = .;

    /DISCARD/ : {
		*(.comment)
        *(.eh_frame)
		*(.note.gnu.build-id)
	}
}
