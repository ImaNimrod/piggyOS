#ifndef	_KERNEL_STRING_H
#define	_KERNEL_STRING_H

#include <memory/kheap.h>
#include <stddef.h>
#include <stdint.h>

/* function declarations */
void *memcpy (void *__restrict, const void *__restrict, size_t);
void *memset (void *, uint32_t, size_t);

int strcmp(const char* str1, const char* str2);
char* strcpy(char* str1, const char* str2); 
char* strdup(const char *s);
size_t strlen(const char *str);
char *strtok_r(char *str, const char *delim, char **saveptr);

#endif
