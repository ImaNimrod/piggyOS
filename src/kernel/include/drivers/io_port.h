#ifndef _KERNEL_IOPORT_H
#define _KERNEL_IOPORT_H

#include <stddef.h>
#include <stdint.h>

/* function declarations */ 
void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);

void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);

void outl(uint16_t port, uint32_t value);
uint32_t inl(uint16_t port);

void outsm(uint16_t port, uint8_t* data, size_t size);
void insm(uint16_t port, uint8_t* data, size_t size);

#endif
