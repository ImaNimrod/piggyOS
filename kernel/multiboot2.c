#include <multiboot2.h>

struct mboot2_tag* mboot2_find_tag(struct mboot2_begin* mb2, uint32_t tag_type) {
    struct mboot2_tag* tag = mb2->tags;
    struct mboot2_tag* prev_tag = tag;

    do {
        if (tag->type == tag_type)
            return tag;

        prev_tag = tag;
        tag = (struct mboot2_tag*) ((uintptr_t) tag + ((tag->size + 7) & ~7));
    } while (prev_tag->type != MBOOT2_TAG_END);

    return NULL;
}
