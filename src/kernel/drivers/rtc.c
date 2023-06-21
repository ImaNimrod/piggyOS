#include <drivers/rtc.h>

time_t dt;

uint8_t read_cmos(cmos_reg reg) {
	outb(0x70, (0x80 | reg));
	return inb(0x71);
}

void write_cmos(cmos_reg reg, uint8_t value) {
	outb(0x70, (0x80 | reg));
	outb(0x71, value);
}

static void rtc_handler(regs_t *r) {
    if (read_cmos(CMOS_STATUS_C) & 0x10) {
        dt.second = from_bcd(read_cmos(CMOS_SECOND));
        dt.minute = from_bcd(read_cmos(CMOS_MINUTE));
        dt.hour   = from_bcd(read_cmos(CMOS_HOUR));
        dt.day    = from_bcd(read_cmos(CMOS_DAY));
        dt.month  = from_bcd(read_cmos(CMOS_MONTH));
        dt.year   = from_bcd(read_cmos(CMOS_YEAR));
    }

    uint8_t c = read_cmos(CMOS_STATUS_C);
	if ((c & CMOS_C_UPDATE_END_INTERRUPT) == 0){
		kprintf("unexpected rtc interrupt; status = %x", c);
	}

    irq_ack(RTC_IRQ);

    (void) r;
}

 
void get_time(time_t *time) {
    memcpy(time, &dt, sizeof(time_t));
}

void rtc_init(void) {
	kprintf("initializing RTC...\n");
 	uint8_t status = read_cmos(CMOS_STATUS_B);
	status |= CMOS_B_UPDATE_END_INTERRUPT;    
    
    irq_install_handler(RTC_IRQ, rtc_handler);

    write_cmos(CMOS_STATUS_B, status);
}
