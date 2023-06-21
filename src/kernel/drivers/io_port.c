#include <drivers/io_port.h>

/* 8 bits of data */
void outb(uint16_t port, uint8_t data) {
	__asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
	return;
}

uint8_t inb(uint16_t port) {
	uint8_t ret;
	__asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

/* 16 bits of data */
void outw(uint16_t port, uint16_t value) {
	__asm__ volatile ("outw %w0, %1" : : "a" (value), "id" (port) );
}

uint16_t inw(uint16_t port){
   uint16_t ret;
   __asm__ volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
} 

/* 32 bits of data */
void outl(uint16_t port, uint32_t value) {
	__asm__ volatile ("outl %%eax, %%dx" :: "d" (port), "a" (value));
}

uint32_t inl(uint16_t port) {
   uint32_t ret;
   __asm__ volatile ("inl %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
} 

void io_wait(void) {
    outb(0x80, 0);
}
