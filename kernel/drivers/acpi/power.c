#include <drivers/acpi.h>

void reboot(void) {
    klog(LOG_WARN, "Rebooting...\n");

    uint8_t tmp = 0x02;
    while (tmp & 0x02)
        tmp = inb(0x64);
    outb(0x64, 0xFE);

    klog(LOG_ERR, "Reboot failed\n");
    __asm__ volatile("hlt");
}

void shutdown(void) {
    klog(LOG_WARN, "Shuting down the machine...\n");

    /* try to shut down any emulators first */
    outw(0xb004, 0x2000); /* bochs, old qemu */
    outw(0x604, 0x2000);  /* new qemu */
    outw(0x4004, 0x3400); /* virtualbox */

    /* TODO ACPI shutdown */
    klog(LOG_ERR, "Shutdown failed\n");
}
