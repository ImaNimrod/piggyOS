#include <drivers/pci.h>

static pci_device **pci_devices = 0;
static uint32_t devs = 0;

static pci_driver **pci_drivers = 0;
static uint32_t drivs = 0;

uint16_t pci_read_word(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset) {
	uint64_t address;
    uint64_t lbus = (uint64_t) bus;
    uint64_t lslot = (uint64_t) slot;
    uint64_t lfunc = (uint64_t) func;
    uint16_t tmp = 0;
    address = (uint64_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t) 0x80000000));

    outl(PCI_CONFIG_ADDRESS, address);
    tmp = (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

void pci_write_word(uint32_t bus, uint32_t slot, uint32_t func, uint16_t offset, uint32_t data) {
	uint32_t address = (uint32_t)((bus << 16) || (slot << 11) |
			(func << 8) | (offset & 0xFC) | ((uint32_t) 0x80000000));
	outl(PCI_CONFIG_ADDRESS, address);
	outl(PCI_CONFIG_DATA, data);
}

uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function) {
        uint32_t r0 = pci_read_word(bus,device,function,0);
        return r0;
}

uint16_t getDeviceID(uint16_t bus, uint16_t device, uint16_t function) {
        uint32_t r0 = pci_read_word(bus,device,function,2);
        return r0;
}

static void add_pci_device(pci_device *pdev) {
    pci_devices[devs] = pdev;
    devs ++;
    return;
}

static void pci_probe(void) {
	for(uint32_t bus = 0; bus < 256; bus++) {
        for(uint32_t slot = 0; slot < 32; slot++) {
            for(uint32_t function = 0; function < 8; function++) {
                    uint16_t vendor = getVendorID(bus, slot, function);
                    if(vendor == 0xffff) continue;
                    uint16_t device = getDeviceID(bus, slot, function);

                    kprintf("vendor: 0x%x device: 0x%x\n", vendor, device);

                    pci_device *pdev = (pci_device *) kmalloc(sizeof(pci_device));
                    pdev->vendor = vendor;
                    pdev->device = device;
                    pdev->func = function;
                    pdev->driver = 0;
                    add_pci_device(pdev);
            }
        }
    }
}

uint16_t pciCheckVendor(uint16_t bus, uint16_t slot) {
    uint16_t vendor;
    if ((vendor = pci_read_word(bus, slot, 0, 0)) != 0xFFFF) {
        return 0;
    } else {
        return (vendor);
    }
}

void pci_init(void) {
	devs = drivs = 0;
	pci_devices = (pci_device **) kmalloc(32 * sizeof(pci_device));
	pci_drivers = (pci_driver **) kmalloc(32 * sizeof(pci_driver));
    klog(LOG_OK, "PCI device support loaded\n");
	pci_probe();
}

void pci_register_driver(pci_driver *driv) {
	pci_drivers[drivs] = driv;
	drivs ++;
	return;
}

void pci_proc_list(void) {
	for(uint32_t i = 0; i < devs; i++) {
		pci_device *pci_dev = pci_devices[i];
		if(pci_dev->driver)
			kprintf("[%x:%x:%x] => %s\n", pci_dev->vendor, pci_dev->device, pci_dev->func, pci_dev->driver->name);
		else
			kprintf("[%x:%x:%x]\n", pci_dev->vendor, pci_dev->device, pci_dev->func);
	}
}
