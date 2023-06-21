#ifndef _KERNEL_SERIAL_H
#define _KERNEL_SERIAL_H

#include <cpu/isr.h>
#include <display.h>
#include <drivers/io_port.h>
#include <stdint.h>

#define PORT_COM1 0x3f8
#define PORT_COM2 0x2f8
#define PORT_COM3 0x3e8
#define PORT_COM4 0x2e8

#define SERIAL_IRQ 36
#define READ_QUEUE_SIZE 1024

/* function declarations */
void serial_init(void);
char serial_getc(void);
void serial_putc(const char c);
void serial_puts(const char* str);

#endif
