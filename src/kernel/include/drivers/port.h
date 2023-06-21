#ifndef _KERNEL_PORT_H
#define _KERNEL_PORT_H

#include <stddef.h>
#include <stdint.h>

/* function definitions */ 

void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);

void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);

void outl(uint16_t port, uint32_t value);
uint32_t inl(uint16_t port);

#endif
