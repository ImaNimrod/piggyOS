#ifndef _KERNEL_RTC_H
#define _KERNEL_RTC_H

#include <cpu/isr.h>
#include <display.h>
#include <drivers/io_port.h>
#include <stdint.h>

#define CMOS_REG  0x70
#define CMOS_DATA 0x71

#define RTC_IRQ 8

#define from_bcd(val) (((val >> 4) * 10) + (val & 0xf))

typedef enum {
	CMOS_SECOND = 0x00,
	CMOS_MINUTE = 0x02,
	CMOS_HOUR = 0x04,
	CMOS_WEEKDAY = 0x06,
	CMOS_DAY = 0x07,
	CMOS_MONTH = 0x08,
	CMOS_YEAR = 0x09,

	CMOS_STATUS_A = 0x0a,
	CMOS_STATUS_B = 0x0b,
	CMOS_STATUS_C = 0x0c
} cmos_reg;

typedef struct{
	uint16_t year, mnth, day, hour, min, sec;
} time_t;

/* function declarations */
uint8_t read_cmos(cmos_reg reg);
void write_cmos(cmos_reg, uint8_t value);
void get_time(time_t *time);
void rtc_init(void);

#endif
