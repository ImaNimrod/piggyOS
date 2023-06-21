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

#endif
