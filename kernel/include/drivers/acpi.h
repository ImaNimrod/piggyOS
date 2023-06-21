#ifndef _KERNEL_ACPI_H
#define _KERNEL_ACPI_H

#include <display.h>
#include <drivers/io_port.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
} __attribute__((packed)) rsdp_t;

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_tbl_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) acpi_header_t;

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
} __attribute__((packed)) fadt_t;

/* function declarations */
void acpi_init(rsdp_t* rsdp);
void reboot(void);
void shutdown(void);

#endif 