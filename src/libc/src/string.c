#include <string.h>

void *memchr (const void *s, uint32_t c, size_t n) {
    const uint8_t uc = c; 
    const uint8_t *su;

    for (su = s; 0 < n; su++, n--) { 
        if (*su == uc) {
            return ((void *) su); 
        }
    }

    return NULL;
}

uint32_t memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *su1, *su2; 

    for (su1 = s1, su2 = s2; 0 < n; su1++, su2++, n--) {
        if (*su1 != *su2) {
            return ((*su1 < *su2) ? -1 : +1);
        }
    }
    return 0;
}

void *memcpy (void *s1, const void *s2, size_t n) {
    int8_t *su1; 
    const int8_t *su2; 

    for (su1 = s1, su2 = s2; 0 < n; su1++, su2++, n--) {
        *su1 = *su2;
    }
    return s1;
}

void *memmove (void *s1, const void *s2, size_t n) {
    char *sc1;
    const char *sc2; 

    sc1 = s1; 
    sc2 = s2; 

    if (sc2 < sc1 && sc1 < sc2 + n) {
        for (sc1 += n, sc2 += n; 0 < n; n--) {
            *sc1-- = *sc2--; 
        }
    } else {
        for (; 0 < n; n--)
            *sc1++ = *sc2++;
    }

    return s1;
}

void *memset (void *s, uint32_t c, size_t n) {
    const uint8_t uc = c; 
    uint8_t *su; 
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

char* strcpy(char* str1, char* str2) {
    char *s = str1;
    for (s = str1; (*s++ = *str2++) != '\0';);  
    return str1;
}

size_t strlen(const char *str) {
	size_t i = 0;
	while(str[i] != 0) i++;
	return i;
}
