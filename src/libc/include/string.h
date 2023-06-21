#ifndef	_STRING_H
#define	_STRING_H

#include <stddef.h>
#include <stdint.h>

/* function declarations */

void *memchr (const void *, uint32_t, size_t);
uint32_t memcmp (const void *, const void *, size_t);
void *memcpy (void *__restrict, const void *__restrict, size_t);
void *memmove (void *, const void *, size_t);
void *memset (void *, uint32_t, size_t);

int strcmp(const char* str1, const char* str2);
char* strcpy(char* str1, const char* str2); 
char* strdup(const char *s);
size_t strlen(const char *str);
char *strpbrk(const char *str, const char *accept);
size_t strspn(const char *str, const char *accept);
char* strtok(char *s, const char *delim);
char *strtok_r(char *str, const char *delim, char **saveptr);

#endif
