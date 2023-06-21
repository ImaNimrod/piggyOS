#include <drivers/keyboard.h>

static const char lower_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
    '9', '0', '-', '=', '\b',	/* Backspace */
    '\t',			/* Tab */
    'q', 'w', 'e', 'r',	/* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    0,			/* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
    '\'', '`',   0,		/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
    'm', ',', '.', '/',   0,					/* Right shift */
    '*',
    0,	/* Alt */
    ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

static void keyboard_irq_handler(regs_t* r) {
    uint8_t scancode;

    for (uint16_t i = 0; i < 1000; i++)
        if((inb(PS2_CMD) & 1) == 0) continue;

    scancode = inb(PS2_DATA);

    if (scancode & 0x80) {
    } else {
        kprintf("%c", lower_map[scancode]);
    }

    (void) r;
}

void keyboard_init(void) {
    /* installs keyboard_handler to IRQ1 */ 
    int_install_handler(KEYBOARD_IRQ, keyboard_irq_handler);
    klog(LOG_OK, "PS/2 Keyboard device initialized\n");
}

void keyboard_reset(void) {
	uint8_t tmp = inb(0x61);
	outb(0x61, tmp | 0x80);
	outb(0x61, tmp & 0x7F);
	inb(PS2_DATA);
}
