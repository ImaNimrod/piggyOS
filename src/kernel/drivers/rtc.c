#include <drivers/rtc.h>

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

void get_time(time_t* dt) {
    dt->sec    = from_bcd(read_cmos(CMOS_SECOND));
    dt->min    = from_bcd(read_cmos(CMOS_MINUTE));
    dt->hour   = from_bcd(read_cmos(CMOS_HOUR));
    dt->day    = from_bcd(read_cmos(CMOS_DAY));
    dt->mnth  = from_bcd(read_cmos(CMOS_MONTH));
    dt->year   = from_bcd(read_cmos(CMOS_YEAR));
}
