#include <string.h>

int memcmp(void *s1, void *s2, int len) {
    uint32_t *p = s1;
    uint32_t *q = s2;
    int compare_status = 0;

    if (s1 == s2)
        return compare_status;

    while (len > 0) {
        if (*p != *q) {
            compare_status = (*p >*q)?1:-1;
            break;
        }

        len--;
        p++;
        q++;
    }

    return compare_status;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t count) {
    __asm__ volatile ("cld; rep movsb" : "+c" (count), "+S" (src), "+D" (dest) :: "memory");
	return dest;
}

void *memset (void* s, uint32_t c, size_t n) {
    const uint8_t uc = c; 
    uint8_t* su; 
    for (su = s; 0 < n; ++su, --n) {
        *su = uc; 
    }
    return (s); 
} 

int strcmp(const char* str1, const char* str2) {
    for (; *str1 == *str2; str1++, str2++) {
        if (*str1 == '\0')
            return 0;
    } 
    return (*(uint8_t *) str1 < *(uint8_t *) str2) ? -1 : +1;
}

char* strcpy(char* str1, const char* str2) {
    int len = strlen(str2);
	memcpy(str1, str2, len+1);
	return str1;
}

size_t strlen(const char *str) {
	size_t i = 0;
	while(str[i] != 0) i++;
	return i;
}
