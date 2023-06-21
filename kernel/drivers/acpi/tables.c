#include <drivers/acpi.h>

static uint8_t checksum(acpi_header_t* header) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i < header->length; i++)
        sum += ((char*) header)[i];
    return sum;
}

static void acpi_parse_fadt(fadt_t* fadt) {
    /* locate dsdt */
    acpi_header_t* dsdt = (acpi_header_t*) fadt->dsdt_address;

    if (checksum(dsdt)) {
        klog(LOG_ERR, "ACPI: %s checksum failed\n", &dsdt->signature);
        return;
    }

    char* S5_addr = (char*) dsdt + 36;
    uint32_t dsdt_length = dsdt->length - 36;
    while (0 < dsdt_length--) {
        if (memcmp(S5_addr, "_S5_", 4) == 0)
            break;
        S5_addr++; 
    }
}

static void acpi_parse_headers(acpi_header_t* header) {
    uint32_t signature = header->signature;

    if (checksum(header)) {
        klog(LOG_ERR, "ACPI: %s checksum failed\n", &signature);
        return;
    }

    char sig_string[5];
    memcpy(sig_string, &signature, 4);
    sig_string[4] = 0;

    if (signature == 0x50434146) /* FACP */
        acpi_parse_fadt((fadt_t*) header);
}

static void acpi_parse_rsdt(acpi_header_t* rsdt) {
    if (checksum(rsdt)) {
        klog(LOG_ERR, "ACPI: RSDT checksum failed\n");
        return;
    }

    uint32_t* p = (uint32_t*) (rsdt + 1);
    uint32_t* end = (uint32_t*) ((uint8_t*) rsdt + rsdt->length);

    while (p < end) {
        uint32_t address = *p++;
        acpi_parse_headers((acpi_header_t*) address);
    }
}

void acpi_init(rsdp_t* rsdp) {
    klog(LOG_OK, "Parsing ACPI tables\n");

    uint8_t sum = 0;
    for (uint8_t i = 0; i < 20; ++i)
        sum += ((char *) rsdp)[i];

    if(sum) {
        klog(LOG_ERR, "ACPI: RSDP checksum verification failed\n");
        return;
    }

    if(rsdp->revision == 0) {
        acpi_parse_rsdt((acpi_header_t*) rsdp->rsdt_address);
    } else {
        klog(LOG_ERR, "Unsuppored ACPI version %d\n", rsdp->revision);
    }
}
