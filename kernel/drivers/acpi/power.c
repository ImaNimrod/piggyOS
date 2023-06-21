#include <drivers/acpi.h>

extern uint32_t PM1a_CNT, PM1b_CNT;
extern uint16_t SLP_TYPa, SLP_TYPb;
extern uint16_t SCI_EN, SLP_EN;

void reboot(void) {
    klog(LOG_WARN, "Rebooting the machine...\n");

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

    if (SCI_EN == 0) {
        kprintf("ACPI: shutdown not posible\n");
        return;
    }

    outw(PM1a_CNT, SLP_TYPa | SLP_EN );
    if (PM1b_CNT)
        outw(PM1b_CNT, SLP_TYPb | SLP_EN );

    klog(LOG_ERR, "Shutdown failed\n");
}
