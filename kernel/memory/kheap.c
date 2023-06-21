#include <memory/kheap.h>

struct Block* head = NULL;
struct Block* tail = NULL;
struct Block* freelist = NULL;

void* heap_start;
void* heap_end; 
void* heap_curr;
void* heap_max;

bool kheap_enabled = false;

extern page_directory_t *kpage_dir;
uint32_t placement_address = (uint32_t) &kernel_end;

void *kmalloc_cont(uint32_t sz, int align, uint32_t *phys) {

    if (align == 1 && (placement_address & 0xFFFFF000)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }

    if (phys)
        *phys = placement_address;

    uint32_t tmp = placement_address;
    placement_address += sz;
    return (void*)tmp;
}

uint32_t kmalloc_int(uint32_t sz, int align, uint32_t *phys) {
    if (heap_start != NULL) {
        if(align) sz = sz + 4096;
        void* addr = malloc(sz);
        uint32_t align_addr = ((uint32_t)addr & 0xFFFFF000) + 0x1000;
        if (phys != 0) {
            uint32_t t = (uint32_t) addr;
            if(align)
                t = align_addr;
             *phys = (uint32_t) virt2phys(kpage_dir, (void*)t);

        }
        if(align)
            return align_addr;
        return (uint32_t)addr;
    } else {
        if (align == 1 && (placement_address & 0xFFFFF000) ) {
            placement_address &= 0xFFFFF000;
            placement_address += 0x1000;
        }
        if (phys)
            *phys = placement_address;
        uint32_t tmp = placement_address;
        placement_address += sz;
        return tmp;
    }
}

uint32_t kmalloc_a(uint32_t sz) {
    return kmalloc_int(sz, 1, 0);
}

uint32_t kmalloc_p(uint32_t sz, uint32_t *phys) {
    return kmalloc_int(sz, 0, phys);
}

uint32_t kmalloc_ap(uint32_t sz, uint32_t *phys) {
    return kmalloc_int(sz, 1, phys);
}

void* kmalloc(size_t sz) {
    return (void*) kmalloc_int(sz, 0, 0);
}

void *kcalloc(uint32_t num, uint32_t size) {
    void* ptr = malloc(num* size);
    memset(ptr, 0, num*size);
    return ptr;
}

void* krealloc(void* ptr, uint32_t size) {
    return realloc(ptr, size);
}

void kfree(void* ptr) {
    free(ptr);
}

void kheap_init(void* start, void* end, void* max) {
    heap_start = start;
    heap_end = end;
    heap_max = max;
    heap_curr = start;
    kheap_enabled = true;
}

uint32_t getRealSize(uint32_t size) {
    return (size >> 1) << 1;
}


int isEnd(struct Block* n) {
    return n == tail;
}

int doesItFit(struct Block* n, uint32_t size) {
    return n->size >= getRealSize(size) && isFree(n);
}

void setFree(uint32_t *size, int x) {
    if(x) {
        *size = *size | 1;
        return;
    }
    *size = *size & 0xFFFFFFFE;
}

int isFree(struct Block* n) {
    if(!n) return 0;
    return (n->size & 0x1);
}

void removeNodeFromFreelist(struct Block* x) {
    if(!x) return;
    if(x->prev) {
        x->prev->next = x->next;
        if(x->next)
            x->next->prev = x->prev;
    } else {
        freelist = x->next;
        if(freelist)
            freelist->prev = NULL;
    }
}

void addNodeToFreelist(struct Block * x) {
    if(!x) return;
    x->next = freelist;
    if(freelist)
        freelist->prev = x;
    freelist = x;
    freelist->prev = NULL;
}

struct Block * bestfit(uint32_t size) {
    if(!freelist) return NULL;
    struct Block * curr = freelist;
    struct Block * currBest = NULL;

    while(curr) {
        if(doesItFit(curr, size)) {
            if(currBest == NULL || curr->size < currBest->size)
                currBest = curr;
        }
        curr = curr ->next;
    }
    return currBest;;
}

/*
 * Given a block , find its previous block
 *
 * */
struct Block* getPrevBlock(struct Block* n) {
    if(n == head) return NULL;
    void *ptr = n;
    uint32_t *uptr = (uint32_t*) ptr - sizeof(uint32_t);
    uint32_t prev_size = getRealSize(*uptr);
    void *ret = (uint32_t*) ptr - OVERHEAD - prev_size;
    return ret;
}
/*
 * Given a block , find its next block
 *
 * */
struct Block* getNextBlock(struct Block* n) {
    if(n == tail) return NULL;
    void *ptr = n;
    ptr = (uint32_t*) ptr + OVERHEAD + getRealSize(n->size);
    return ptr;
}

void *malloc(size_t size) {
    if(size == 0) return NULL;

    uint32_t roundedSize = ((size + 15) / 16) * 16;                                  /// think twice how you round
    uint32_t blockSize = roundedSize + OVERHEAD;
    struct Block* best;
    best = bestfit(roundedSize);

    uint32_t* trailingSize = NULL;
    if(best) {
        void* ptr = (struct Block*) best;
        void* saveNextBlock = getNextBlock(best);
        uint32_t chunkSize = getRealSize(best->size) + OVERHEAD;
        uint32_t rest = chunkSize - blockSize; // what's left
        uint32_t whichSize;

        if(rest < 8 + OVERHEAD) whichSize = chunkSize;
        else whichSize = blockSize;
        best->size = whichSize - OVERHEAD;
        setFree(&(best->size), 0);
        void* base = ptr;
        trailingSize = (uint32_t*) ptr + whichSize - sizeof(uint32_t);
        *trailingSize = best->size;
        ptr = (uint32_t*) (trailingSize + 1);

        if(rest < 8 + OVERHEAD) goto noSplit;

        if(rest >= 8) {
            if(base != tail && isFree(saveNextBlock)) {
                void* nextblock = saveNextBlock;
                struct Block* n_nextblock = nextblock;
                removeNodeFromFreelist(n_nextblock);

                struct Block* t = ptr;
                t->size = rest - OVERHEAD + getRealSize(n_nextblock->size) + OVERHEAD;
                setFree(&(t->size), 1);

                ptr = (uint32_t*) ptr + sizeof(struct Block) + getRealSize(t->size);
                trailingSize = ptr;
                *trailingSize = t->size;

                if(nextblock == tail)
                    tail = t;
                addNodeToFreelist(t);
            } else {
                struct Block* putThisBack = ptr;
                putThisBack->size = rest - OVERHEAD;
                setFree(&(putThisBack->size), 1);
                trailingSize = (uint32_t*) ptr + sizeof(struct Block) + getRealSize(putThisBack->size);
                *trailingSize = putThisBack->size;
                if(base == tail)
                    tail = putThisBack;
                addNodeToFreelist(putThisBack);
            }
        }
noSplit:
        removeNodeFromFreelist(base);
        return (uint32_t*) base + sizeof(struct Block);
    } else {
        uint32_t realsize = blockSize;
        struct Block* ret = ksbrk(realsize);
        if(!head) head = ret;
        void* ptr = ret;
        void* save = ret;
        tail = ptr;

        ret->size = blockSize - OVERHEAD;
        setFree(&(ret->size), 0);
        ptr = (uint32_t*) ptr + blockSize - sizeof(uint32_t);
        trailingSize = ptr;
        *trailingSize = ret->size;
        return (size_t*) save + sizeof(struct Block);
    }
}

void free(void *ptr) {
    struct Block* curr = (struct Block*) ptr - sizeof(struct Block);
    struct Block* prev = getPrevBlock(curr);
    struct Block* next = getNextBlock(curr);

    if(isFree(prev) && isFree(next)) {
        prev->size = getRealSize(prev->size) + 2*OVERHEAD + getRealSize(curr->size) + getRealSize(next->size);
        setFree(&(prev->size), 1);
        uint32_t* trailingSize = (uint32_t*) prev + sizeof(struct Block) + getRealSize(prev->size);
        *trailingSize = prev->size;
        if(tail == next) tail = prev;
        removeNodeFromFreelist(next);
    } else if(isFree(prev)) {
        prev->size = getRealSize(prev->size) + OVERHEAD + getRealSize(curr->size);
        setFree(&(prev->size), 1);
        uint32_t* trailingSize = (uint32_t*) prev + sizeof(struct Block) + getRealSize(prev->size);
        *trailingSize = prev->size;
        if(tail == curr) tail = prev;
    } else if(isFree(next)) {
        curr->size = getRealSize(curr->size) + OVERHEAD + getRealSize(next->size);
        setFree(&(curr->size), 1);
        uint32_t* trailingSize = (uint32_t*) curr + sizeof(struct Block) + getRealSize(curr->size);
        *trailingSize = curr->size;

        if(tail == next) tail = curr;
        removeNodeFromFreelist(next);
        addNodeToFreelist(curr);
    } else {
        setFree(&(curr->size), 1);
        uint32_t* trailingSize = (uint32_t*) curr + sizeof(struct Block) + getRealSize(curr->size);
        *trailingSize = curr->size;
        addNodeToFreelist(curr);
    }
}

void *realloc(void *ptr, uint32_t size) {
    uint32_t *trailingSize = NULL;
    if(!ptr) return malloc(size);
    if(size == 0 && ptr != NULL) {
        free(ptr);
        return NULL;
    }
    uint32_t roundedSize = ((size + 15)/16) * 16;                                  /// think twice how you round
    uint32_t blockSize = roundedSize + OVERHEAD;
    struct Block* nextBlock, *prevBlock;
    struct Block* nptr = (struct Block*) ptr - sizeof(struct Block);

    nextBlock = getNextBlock(nptr);
    prevBlock = getPrevBlock(nptr);
    if(nptr->size == size) return ptr;
    if(nptr->size < size) {
        if(tail != nptr && isFree(nextBlock) && (getRealSize(nptr->size) + OVERHEAD + getRealSize(nextBlock->size)) >= roundedSize) {
            removeNodeFromFreelist(nextBlock);
            nptr->size = getRealSize(nptr->size) + OVERHEAD + getRealSize(nextBlock->size);
            setFree(&(nptr->size), 0);
            trailingSize = (uint32_t*) nptr + sizeof(struct Block) + getRealSize(nptr->size);
            *trailingSize = nptr->size;
            if(tail == nextBlock) {
                tail = nptr;
            }
            return nptr + 1;
        } else if(head != nptr && isFree(prevBlock) && (getRealSize(nptr->size) + OVERHEAD + getRealSize(prevBlock->size)) >= roundedSize) {
            uint32_t originalSize = getRealSize(nptr->size);
            removeNodeFromFreelist(prevBlock);
            prevBlock->size = originalSize + OVERHEAD + getRealSize(prevBlock->size);
            setFree(&(prevBlock->size), 0);
            trailingSize = (uint32_t*) prevBlock + sizeof(struct Block) + getRealSize(prevBlock->size);
            *trailingSize = prevBlock->size;
            if(tail == nptr) {
                tail = prevBlock;
            }
            memcpy(prevBlock+1, ptr, originalSize);
            return prevBlock + 1;
        }
        void* newplace = malloc(size);
        memcpy(newplace, ptr, getRealSize(nptr->size));

        free(ptr);
        return newplace;
    } else {
        uint32_t rest = getRealSize(nptr->size) + OVERHEAD - blockSize;
        if(rest < 8 + OVERHEAD) return ptr;

        nptr->size = blockSize - OVERHEAD;
        setFree(&(nptr->size), 0);
        trailingSize = (uint32_t*) nptr + sizeof(struct Block) + getRealSize(nptr->size);
        *trailingSize = nptr->size;

        struct Block* splitBlock = (struct Block*) trailingSize + sizeof(uint32_t);

        if(nextBlock && isFree(nextBlock)) {
            splitBlock->size = rest + getRealSize(nextBlock->size);
            setFree(&(splitBlock->size), 1);
            trailingSize = (uint32_t*) splitBlock + sizeof(struct Block) + getRealSize(splitBlock->size);
            *trailingSize = splitBlock->size;

            removeNodeFromFreelist(nextBlock);
            if(tail == nextBlock) {
                tail = splitBlock;
            }
            addNodeToFreelist(splitBlock);

            return ptr;
        }
        splitBlock->size = rest - OVERHEAD;
        setFree(&(splitBlock->size), 1);
        trailingSize = (uint32_t*) splitBlock + sizeof(struct Block) + getRealSize(splitBlock->size);
        *trailingSize = splitBlock->size;
        addNodeToFreelist(splitBlock);

        return ptr;
    }
}
