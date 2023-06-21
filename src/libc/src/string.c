#include <string.h>
#include "../../kernel/include/memory.h"

void *memchr(const void *s, uint32_t c, size_t n) {
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

char *strpbrk(const char *str, const char *accept) {
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

size_t strspn(const char *str, const char *accept) {
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

