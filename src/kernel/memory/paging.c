#include <memory.h>

static uint32_t* page_directory = 0;
static uint32_t page_dir_loc = 0;
static uint32_t* last_page = 0;

static void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys) {
	uint16_t id = virt >> 22;
	for(int i = 0; i < 1024; i++) {
		last_page[i] = phys | 3;
		phys += 4096;
	}
	page_directory[id] = ((uint32_t) last_page) | 3;
	last_page = (uint32_t *) (((uint32_t) last_page) + 4096);
	kprintf("mapping 0x%x (%d) to 0x%x\n", virt, id, phys);
}

static void paging_enable(void) {
	__asm__ volatile("mov %%eax, %%cr3": :"a" (page_dir_loc));	
	__asm__ volatile("mov %cr0, %eax");
	__asm__ volatile("orl $0x80000000, %eax");
	__asm__ volatile("mov %eax, %cr0");
}

void paging_init(void) {
	kprintf("setting up paging...\n");
	page_directory = (uint32_t*) 0x400000;
	page_dir_loc = (uint32_t) page_directory;
	last_page = (uint32_t *) 0x404000;
	for(int i = 0; i < 1024; i++) {
		page_directory[i] = 0 | 2;
	}
	paging_map_virtual_to_phys(0, 0);
	paging_map_virtual_to_phys(0x400000, 0x400000);
	paging_enable();
	kprintf("paging successfully enabled\n");
}
