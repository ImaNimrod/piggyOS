#ifndef _KERNEL_SERIAL_H
#define _KERNEL_SERIAL_H

#include <display.h>
#include <drivers/io_port.h>
#include <stdint.h>

#define PORT_COM1 0x3f8
#define PORT_COM2 0x2f8
#define PORT_COM3 0x3e8
#define PORT_COM4 0x2e8

/* function declarations */
void serial_init(void);
char serial_getc(uint16_t port);
void serial_putc(uint16_t port, const char c);
void serial_puts(uint16_t port, const char *str);

#endif
