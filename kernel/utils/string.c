#include <string.h>

int memcmp(void *s1, void *s2, int len) {
    uint32_t *p = s1;
    uint32_t *q = s2;
    int charCompareStatus = 0;
    //If both pointer pointing same memory block
    if (s1 == s2)
        return charCompareStatus;

    while (len > 0) {
        if (*p != *q) {
            charCompareStatus = (*p >*q)?1:-1;
            break;
        }

        len--;
        p++;
        q++;
    }
    return charCompareStatus;
}

void* memcpy(void* dest, const void* src, size_t n) {
	__asm__ volatile("cld; rep movsb" : "=c"((int){0}) : "D"(dest), "S"(src), "c"(n) : "flags", "memory");
	return dest;
}

void* memmove(void* restrict dest, const void* restrict src, size_t count) {
	size_t i;
	uint8_t *a = dest;
	const uint8_t *b = src;
	if (src < dest) {
		for ( i = count; i > 0; --i)
			a[i-1] = b[i-1];
	} else {
		for ( i = 0; i < count; ++i)
			a[i] = b[i];
	}
	return dest;
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

char* strcpy(char* str1, char const* str2) {
    char* s = str1;
    while ((*str1++ = *str2++) != '\0');
    return s;
}

char* strdup(const char *s) {
  size_t len = strlen(s) + 1;
  void *new = kmalloc(len);

  if (new == NULL)
    return NULL;

  return (char *) memcpy(new, s, len);
}

size_t strlen(const char *str) {
	size_t i = 0;
	while(str[i] != 0) i++;
	return i;
}

static char *strpbrk(const char *str, const char *accept) {
	const char *acc = accept;

	if (!*str) {
		return NULL;
	}

	while (*str) {
		for (acc = accept; *acc; ++acc) {
			if (*str == *acc)
				break;
		}
		if (*acc)
            break;
		++str;
	}

	if (*acc == '\0') {
		return NULL;
	}

	return (char *)str;
}

static size_t strspn(const char *str, const char *accept) {
	const char *ptr = str;
	const char *acc;

	while (*str) {
		for (acc = accept; *acc; ++acc) {
			if (*str == *acc) {
				break;
			}
		}
		if (*acc == '\0') {
			break;
		}
		str++;
	}
	return str - ptr;
}

static size_t __lfind(const char * str, const char accept) {
    size_t i = 0;
    while ( str[i] != accept) {
        i++;
    }
    return (size_t)(str) + i;
}

char *strtok_r(char *str, const char *delim, char **saveptr) {
	char * token;
	if (str == NULL) {
		str = *saveptr;
	}

	str += strspn(str, delim);
	if (*str == '\0') {
		*saveptr = str;
		return NULL;
	}

	token = str;
	str = strpbrk(token, delim);
	if (str == NULL) {
		*saveptr = (char *) __lfind(token, '\0');
	} else {
		*str = '\0';
		*saveptr = str + 1;
	}
	return token;
}
