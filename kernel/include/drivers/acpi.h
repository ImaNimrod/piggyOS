#ifndef _KERNEL_ACPI_H
#define _KERNEL_ACPI_H

#include <display.h>
#include <drivers/io_port.h>
#include <drivers/timer.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint32_t signature;
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_tbl_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} acpi_header_t;

typedef struct {
    acpi_header_t header;
    uint32_t sdt_entries[];
} rsdt_t;

typedef struct {
    acpi_header_t header;
    uint64_t sdt_entries[];
} xsdt_t;

typedef struct  {
    acpi_header_t header;
    uint32_t firmware_control;
    uint32_t dsdt_address;
    uint8_t reserved;
    uint8_t preferred_pm_profile;
    uint16_t sci_int;
    uint32_t smi_cmd;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t who_cares[64 - 54];
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
    uint8_t who_cares2[89 - 72];
    uint8_t pm1_cnt_length;
} fadt_t;

/* function declarations */
void acpi_init(uint8_t* rsdp);
void reboot(void);
void shutdown(void);

#endif 
