#ifndef _KERNEL_RTC_H
#define _KERNEL_RTC_H

#include <cpu/irq.h>
#include <display.h>
#include <drivers/io_port.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define array_len(xs) (int) (sizeof(xs) / sizeof(xs[0]))
#define from_bcd(val) (uint8_t) ((val >> 4) * 10 + (val & 0xf))

#define RTC_IRQ 8

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

typedef enum{
	CMOS_B_24_HOUR_MODE = (1 << 1),
	CMOS_B_BINARY_MODE = (1 << 2),
	CMOS_B_UPDATE_END_INTERRUPT = (1 << 4)
} cmos_status_b;

typedef enum{
	CMOS_C_UPDATE_END_INTERRUPT = (1 << 4)
} cmos_status_c;

typedef struct{
	uint8_t year, month, day, hour, minute, second;
} time_t;

/* function declarations */
void rtc_init(void);
void get_time(time_t *time);
uint8_t read_cmos(cmos_reg reg);
void write_cmos(cmos_reg reg, uint8_t value);

#endif
