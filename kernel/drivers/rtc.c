#include <drivers/rtc.h>

time_t dt;

uint8_t read_cmos(cmos_reg reg) {
	outb(CMOS_REG, (0x80 | reg));
	return inb(CMOS_DATA);
}

void write_cmos(cmos_reg reg, uint8_t value) {
    __asm__ volatile("cli");
	outb(CMOS_REG, (0x80 | reg));
	outb(CMOS_DATA, value);
    __asm__ volatile("sti");
}

void get_time(time_t* time) {
    memcpy(time, &dt, sizeof(time_t));
}

uint32_t get_seconds(void) {
    uint32_t seconds = ((dt.year * 31536000) + (dt.mnth * 2629800) + (dt.day * 86400)
                       + (dt.hour * 3600) + (dt.min * 60) + dt.sec);
    return seconds;
}

static void rtc_irq_handler(regs_t *r) {
    if (read_cmos(CMOS_STATUS_C) & 0x10) {
        dt.sec   = from_bcd(read_cmos(CMOS_SECOND));
        dt.min   = from_bcd(read_cmos(CMOS_MINUTE));
        dt.hour  = from_bcd(read_cmos(CMOS_HOUR));
        dt.day   = from_bcd(read_cmos(CMOS_DAY));
        dt.mnth  = from_bcd(read_cmos(CMOS_MONTH));
        dt.year  = 2000 + from_bcd(read_cmos(CMOS_YEAR));
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

    if (read_cmos(CMOS_STATUS_C) & 0x10) {
        dt.sec   = from_bcd(read_cmos(CMOS_SECOND));
        dt.min   = from_bcd(read_cmos(CMOS_MINUTE));
        dt.hour  = from_bcd(read_cmos(CMOS_HOUR));
        dt.day   = from_bcd(read_cmos(CMOS_DAY));
        dt.mnth  = from_bcd(read_cmos(CMOS_MONTH));
        dt.year  = 2000 + from_bcd(read_cmos(CMOS_YEAR));
    }

    int_install_handler(RTC_IRQ, rtc_irq_handler);


    klog(LOG_OK, "RTC initialized\n");
}
