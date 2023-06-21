#ifndef _KERNEL_IOPORT_H
#define _KERNEL_IOPORT_H

#include <stddef.h>
#include <stdint.h>

/* function declarations */ 

/* 8 bits of data */
static inline void outb(uint16_t port, uint8_t data) {
	__asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* 16 bits of data */
static inline void outw(uint16_t port, uint16_t data) {
    __asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port) );
}

static inline uint16_t inw(uint16_t port){
   uint16_t ret;
   __asm__ volatile("inw %1, %0" : "=a" (ret) : "Nd" (port));
   return ret;
} 

/* 32 bits of data */
static inline void outl(uint16_t port, uint32_t data) {
	__asm__ volatile("outl %%eax, %%dx" :: "d" (port), "a" (data));
}

static inline uint32_t inl(uint16_t port) {
   uint32_t ret;
   __asm__ volatile("inl %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

#endif
