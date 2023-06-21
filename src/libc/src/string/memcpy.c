#include <string.h>

void *memcpy (void *s1, const void *s2, size_t n) {
    int8_t *su1; 
    const int8_t *su2; 

    for (su1 = s1, su2 = s2; 0 < n; su1++, su2++, n--) {
        *su1 = *su2;
    }
    return s1;
}
