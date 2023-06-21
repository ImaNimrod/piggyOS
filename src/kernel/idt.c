#include <idt.h>

static idt_desc_t idt[IDT_LEN];
idt_ptr_t idt_ptr;

void *memset(void *dest, int c, size_t n)
{
    unsigned char *s = dest;
    size_t k;

    /* Fill head and tail with minimal branching. Each
     * conditional ensures that all the subsequently used
     * offsets are well-defined and in the dest region. */

    if (!n) return dest;
    s[0] = c;
    s[n-1] = c;
    if (n <= 2) return dest;
    s[1] = c;
    s[2] = c;
    s[n-2] = c;
    s[n-3] = c;
    if (n <= 6) return dest;
    s[3] = c;
    s[n-4] = c;
    if (n <= 8) return dest;

    /* Advance pointer to align it at a 4-byte boundary,
     * and truncate n to a multiple of 4. The previous code
     * already took care of any head/tail that get cut off
     * by the alignment. */

    k = -(uintptr_t)s & 3;
    s += k;
    n -= k;
    n &= -4;

#ifdef __GNUC__
    typedef uint32_t __attribute__((__may_alias__)) u32;
    typedef uint64_t __attribute__((__may_alias__)) u64;

    u32 c32 = ((u32)-1)/255 * (unsigned char)c;

    /* In preparation to copy 32 bytes at a time, aligned on
     * an 8-byte bounary, fill head/tail up to 28 bytes each.
     * As in the initial byte-based head/tail fill, each
     * conditional below ensures that the subsequent offsets
     * are valid (e.g. !(n<=24) implies n>=28). */

    *(u32 *)(s+0) = c32;
    *(u32 *)(s+n-4) = c32;
    if (n <= 8) return dest;
    *(u32 *)(s+4) = c32;
    *(u32 *)(s+8) = c32;
    *(u32 *)(s+n-12) = c32;
    *(u32 *)(s+n-8) = c32;
    if (n <= 24) return dest;
    *(u32 *)(s+12) = c32;
    *(u32 *)(s+16) = c32;
    *(u32 *)(s+20) = c32;
    *(u32 *)(s+24) = c32;
    *(u32 *)(s+n-28) = c32;
    *(u32 *)(s+n-24) = c32;
    *(u32 *)(s+n-20) = c32;
    *(u32 *)(s+n-16) = c32;

    /* Align to a multiple of 8 so we can fill 64 bits at a time,
     * and avoid writing the same bytes twice as much as is
     * practical without introducing additional branching. */

    k = 24 + ((uintptr_t)s & 4);
    s += k;
    n -= k;

    /* If this loop is reached, 28 tail bytes have already been
     * filled, so any remainder when n drops below 32 can be
     * safely ignored. */

    u64 c64 = c32 | ((u64)c32 << 32);
    for (; n >= 32; n-=32, s+=32) {
        *(u64 *)(s+0) = c64;
        *(u64 *)(s+8) = c64;
        *(u64 *)(s+16) = c64;
        *(u64 *)(s+24) = c64;
    }
#else
    /* Pure C fallback with no aliasing violations. */
    for (; n; n--, s++) *s = c;
#endif

    return dest;
}

static void idt_set_desc(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].isr_low = base & 0xFFFF;
    idt[num].isr_high = (base >> 16) & 0xFFFF;

    idt[num].sel = sel;
    idt[num].zero = 0;

    idt[num].flags = flags;
}

static void idt_set(idt_ptr_t* idt_ptr) {
    __asm__ volatile("lidtl (idt_ptr)");
    __asm__ volatile("sti");

    (void) idt_ptr;
}

void init_idt(void) {
    idt_ptr.limit = sizeof(idt_desc_t) * IDT_LEN -1;
    idt_ptr.base  = (uint32_t) &idt;

    memset(&idt, 0, sizeof(idt_desc_t) * IDT_LEN);

    idt_set(&idt_ptr);
}

