#include <drivers/serial.h>

static int port_enable(uint16_t port) {
    outb(port + 1, 0x00);
    outb(port + 3, 0x80);
    outb(port + 0, 0x03);
    outb(port + 1, 0x00);
    outb(port + 3, 0x03);
    outb(port + 2, 0xc7);
    outb(port + 4, 0x0b);
    outb(port + 4, 0x1e);
    outb(port + 0, 0xae);

    if(inb(port + 0) != 0xae) {
        return 0;
    }

    outb(port + 4, 0x0b);
    return 1;
}

static int port_read_ready(uint16_t port) {
    return inb(port + 5) & 1;
}

static int port_write_ready(uint16_t port) {
    return inb(port + 5) & 0x20;
}

char serial_getc(uint16_t port) {
	while(port_read_ready(port) == 0) ;
	return inb(port);
}

void serial_putc(uint16_t port, const char c) {
	while(port_write_ready(port) == 0);
	outb(port, c);
}

void serial_puts(uint16_t port, const char* str) {
	for (uint32_t i = 0; i < strlen(str); ++i)
		serial_putc(port, str[i]);
}

void serial_init(void) {
    if (!port_enable(PORT_COM1)) {
        klog(LOG_ERR, "COM1 initialization failed\n");
        return;
    }
    klog(LOG_OK, "Serial Ports initialized\n");
}
