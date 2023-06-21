#include <drivers/rtc.h>

uint8_t read_cmos(cmos_reg reg) {
	outb(0x70, (0x80 | reg));
	return inb(0x71);
}

void write_cmos(cmos_reg reg, uint8_t value) {
    disable_int();
	outb(0x70, (0x80 | reg));
	outb(0x71, value);
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
