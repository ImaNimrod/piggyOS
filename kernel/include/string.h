#ifndef	_KERNEL_STRING_H
#define	_KERNEL_STRING_H

#include <memory/kheap.h>
#include <stddef.h>
#include <stdint.h>

/* function declarations */
void* memcpy(void* restrict dest, const void* restrict src, size_t n);
void* memset (void*, uint32_t, size_t);
int memcmp(void* s1, void* s2, int len);

int strcmp(const char* str1, const char* str2);
char* strcpy(char* str1, const char* str2); 
size_t strlen(const char* str);

#endif
