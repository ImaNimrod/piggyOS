#include <drivers/rtc.h>

time_t dt;

uint8_t read_cmos(cmos_reg reg) {
	outb(CMOS_REG, (0x80 | reg));
	return inb(CMOS_DATA);
}

void write_cmos(cmos_reg reg, uint8_t value) {
    disable_int();
	outb(CMOS_REG, (0x80 | reg));
	outb(CMOS_DATA, value);
    enable_int();
}

void get_time(time_t *time) {
    memcpy(time, &dt, sizeof(time_t));
}

static void rtc_irq_handler(regs_t *r) {
    if (read_cmos(CMOS_STATUS_C) & 0x10) {
        dt.sec   = from_bcd(read_cmos(CMOS_SECOND));
        dt.min   = from_bcd(read_cmos(CMOS_MINUTE));
        dt.hour  = from_bcd(read_cmos(CMOS_HOUR));
        dt.day   = from_bcd(read_cmos(CMOS_DAY));
        dt.mnth  = from_bcd(read_cmos(CMOS_MONTH));
        dt.year  = from_bcd(read_cmos(CMOS_YEAR));
    }

    (void) r;
}

void rtc_init(void) {
    uint8_t status;
     
    status = read_cmos(CMOS_STATUS_B);
    status |=  0x02;             // 24 hour clock
    status |=  0x10;             // update ended interrupts
    status &= ~0x20;             // no alarm interrupts
    status &= ~0x40;             // no periodic interrupt
    write_cmos(0x0B, status);
 
    read_cmos(CMOS_STATUS_C);

    int_install_handler(RTC_IRQ, rtc_irq_handler);

    klog(LOG_OK, "RTC initialized\n");
}
