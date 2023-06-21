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
