#include <drivers/serial.h>

static char read_queue[READ_QUEUE_SIZE];
static int queue_front = -1, queue_rear = -1;

static int port_enable(uint16_t port) {
    outb(port + 3, 0x80);
    outb(port + 0, 0x03);
    outb(port + 1, 0x00); outb(port + 3, 0x03); outb(port + 2, 0xc7);
    outb(port + 4, 0x0b);
    outb(port + 4, 0x1e);
    outb(port + 0, 0xae);

    if(inb(port + 0) != 0xae) {
        return 0;
    }

    outb(port + 4, 0x0b);
    return 1;
}

static inline int port_read_ready(uint16_t port) {
    return inb(PORT_COM1+ 5) & 1;
}

static inline int port_write_ready(uint16_t port) {
    return inb(PORT_COM1 + 5) & 0x20;
}

char serial_getc(void) {
    char c;
    if (queue_front == -1)
        return 0;

    c = read_queue[queue_front];

    if (queue_front == queue_rear) {
        queue_front = queue_rear = -1;
    } else {
        queue_front = (queue_front + 1) % READ_QUEUE_SIZE;
    }
    
    return c;
}

void serial_putc(const char c) {
	while(port_write_ready(PORT_COM1) == 0);
	outb(PORT_COM1, c);
}

void serial_puts(const char* str) {
    while(*str) {
        serial_putc(*str);
        str++;
    }
}

void serial_irq_handler(regs_t* r) {
    (void) r;

    uint32_t isr = inb(PORT_COM1 + 2);
    if (isr & (1 << 2)) {
        while(port_read_ready(PORT_COM1) == 0);
        if ((queue_front == 0 && queue_rear == READ_QUEUE_SIZE -1))
            return;

        if (queue_front == -1) queue_front = 0;
        queue_rear = (queue_rear + 1) % READ_QUEUE_SIZE;
        char d = inb(PORT_COM1);
        read_queue[queue_rear] = d;
    }
}

void serial_init(void) {
    if (!port_enable(PORT_COM1)) {
        klog(LOG_ERR, "COM1 initialization failed\n");
        return;
    }
    
    int_install_handler(SERIAL_IRQ, serial_irq_handler);
    outb(PORT_COM1 + 1, 0x01); // enable interrupts on byte received

    klog(LOG_OK, "Serial Port(s) initialized\n");
}
