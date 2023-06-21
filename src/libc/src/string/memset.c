#include <string.h>

void *memset (void *s, uint32_t c, size_t n) {
    const uint8_t uc = c; 
    uint8_t *su; 
    for (su = s; 0 < n; ++su, --n) {
        *su = uc; 
    }
    return (s); 
} 
