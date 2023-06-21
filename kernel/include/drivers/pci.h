#ifndef _KERNEL_PCI_H
#define _KERNEL_PCI_H

#include <stddef.h>
#include <stdint.h>
#include <display.h>
#include <memory/kheap.h>
#include <drivers/io_port.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

struct __pci_driver;

typedef struct {
	uint32_t vendor;
	uint32_t device;
	uint32_t func;
	struct __pci_driver *driver;
} pci_device;

typedef struct {
	uint32_t vendor;
	uint32_t device;
	uint32_t func;
} pci_device_id;

typedef struct __pci_driver {
	pci_device_id *table;
	char *name;
	uint8_t (*init_one)(pci_device *);
	uint8_t (*init_driver)(void);
	uint8_t (*exit_driver)(void);
} pci_driver;

/* function declarations */ 
void pci_init(void);
void pci_proc_list(void);
uint16_t pci_read_word(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset);
void pci_write_word(uint32_t bus, uint32_t slot, uint32_t func, uint16_t offset, uint32_t data);
       

#endif 
