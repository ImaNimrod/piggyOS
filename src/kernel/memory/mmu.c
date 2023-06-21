#include <memory.h>

static uint32_t last_alloc = 0;
static uint32_t heap_end = 0;
static uint32_t heap_begin = 0;
static uint32_t pheap_begin = 0;
static uint32_t pheap_end = 0;
static uint8_t *pheap_desc = 0;
static uint32_t memory_used = 0;

void mmu_init(uintptr_t* kernel_end) {
	last_alloc = kernel_end + 0x1000;
	heap_begin = last_alloc;
	pheap_end = 0x400000;
	pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
	heap_end = pheap_begin;
	memset((char *) heap_begin, 0, heap_end - heap_begin);
	pheap_desc = (uint8_t *) kmalloc(MAX_PAGE_ALIGNED_ALLOCS);
	kprintf("kernel heap begins at 0x%x\n", last_alloc);
}


void mmu_print_state(void) {
	kprintf("Memory used: %d bytes\n", memory_used);
	kprintf("Memory free: %d bytes\n", heap_end - heap_begin - memory_used);
	kprintf("Heap size: %d bytes\n", heap_end - heap_begin);
	kprintf("Heap start: 0x%x\nHeap end: 0x%x\n", heap_begin, heap_end);
	kprintf("PHeap start: 0x%x\nPHeap end: 0x%x\n", pheap_begin, pheap_end);
}

// char* kmalloc(size_t size) {
//     if(!size) return 0;
//
//     uint8_t *mem = (uint8_t *) heap_begin;
//     while((uint32_t) mem < last_alloc) {
//         alloc_t *a = (alloc_t *)mem;
//         if(!a->size)
//             goto nalloc;
//
//         if(a->status) {
//             mem += a->size;
//             mem += sizeof(alloc_t);
//             mem += 4;
//             continue;
//         }
//
//         if(a->size >= size) {
//             a->status = 1;
//
//             // kprintf("RE:allocated %d bytes from 0x%x to 0x%x\n", size, mem + sizeof(alloc_t), mem + sizeof(alloc_t) + size);
//             memset(mem + sizeof(alloc_t), 0, size);
//             memory_used += size + sizeof(alloc_t);
//             return (char *)(mem + sizeof(alloc_t));
//         }
//
//         mem += a->size;
//         mem += sizeof(alloc_t);
//         mem += 4;
//     }
//
//     nalloc:;
//        if(last_alloc+size+sizeof(alloc_t) >= heap_end) {
//            kpanic("cannot allocate %d bytes! Out of memory.\n", size);
//        }
//        alloc_t *alloc = (alloc_t *)last_alloc;
//        alloc->status = 1;
//        alloc->size = size;
//
//        last_alloc += size;
//        last_alloc += sizeof(alloc_t);
//        last_alloc += 4;
//        // kprintf("allocated %d bytes from 0x%x to 0x%x\n", size, (uint32_t) alloc + sizeof(alloc_t), last_alloc);
//        memory_used += size + 4 + sizeof(alloc_t);
//        memset((char *)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
//        return (char *)((uint32_t)alloc + sizeof(alloc_t));
// }

char* kmalloc(size_t size) {
	if(!size) return NULL;

	uint8_t *mem = (uint8_t *) heap_begin;
	while((uint32_t) mem < last_alloc) {
		alloc_t *a = (alloc_t *)mem;
		if(!a->size)
			goto nalloc;
		if(a->status) {
			mem += a->size;
			mem += sizeof(alloc_t);
			mem += 4;
			continue;
		}

		if(a->size >= size) {
			a->status = 1;

			memset(mem + sizeof(alloc_t), 0, size);
			memory_used += size + sizeof(alloc_t);
			return (char *)(mem + sizeof(alloc_t));
		}
		mem += a->size;
		mem += sizeof(alloc_t);
		mem += 4;
	}

	nalloc:;
	if(last_alloc+size+sizeof(alloc_t) >= heap_end) {
		kpanic("Cannot allocate %d bytes! Out of memory.\n", size);
	}
	alloc_t *alloc = (alloc_t *)last_alloc;
	alloc->status = 1;
	alloc->size = size;

	last_alloc += size;
	last_alloc += sizeof(alloc_t);
	last_alloc += 4;
	memory_used += size + 4 + sizeof(alloc_t);
	memset((char *)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
	return (char *)((uint32_t)alloc + sizeof(alloc_t));
}

void kfree(void *mem) {
	alloc_t *alloc = (mem - sizeof(alloc_t));
	memory_used -= alloc->size + sizeof(alloc_t);
	alloc->status = 0;
}

char* kpmalloc(void) {
    for(int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++) {
        if(pheap_desc[i]) continue;
        pheap_desc[i] = 1;
        kprintf("Pallocated from 0x%x to 0x%x\n", pheap_begin + i*4096, pheap_begin + (i+1)*4096);
        return (char *)(pheap_begin + i*4096);
    }
    kprintf("kpmalloc: FATAL: failure!\n");
    return 0;
}

void kpfree(void *mem) {
	if ((mem < pheap_begin) || (mem > pheap_end)) return;
	uint32_t ad = (uint32_t)mem;
	ad -= pheap_begin;
	ad /= 4096;
	pheap_desc[ad] = 0;
	return;
}

