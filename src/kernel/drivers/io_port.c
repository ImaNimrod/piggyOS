#include <drivers/io_port.h>

/* 8 bits of data */
static inline void outb(uint16_t port, uint8_t data) {
	__asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
	return;
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	__asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

/* 16 bits of data */
static inline void outw(uint16_t port, uint16_t value) {
	__asm__ volatile ("outw %w0, %1" : : "a" (value), "id" (port) );
}

static inline uint16_t inw(uint16_t port){
   uint16_t ret;
   __asm__ volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
} 

/* 32 bits of data */
static inline void outl(uint16_t port, uint32_t value) {
	__asm__ volatile ("outl %%eax, %%dx" :: "d" (port), "a" (value));
}

static inline uint32_t inl(uint16_t port) {
   uint32_t ret;
   __asm__ volatile ("inl %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
} 
