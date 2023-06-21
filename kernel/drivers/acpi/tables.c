#include <drivers/acpi.h>

uint32_t SMI_CMD;
uint8_t ACPI_ENABLE;
uint8_t ACPI_DISABLE;
uint32_t PM1a_CNT;
uint32_t PM1b_CNT;
uint16_t SLP_TYPa;
uint16_t SLP_TYPb;
uint16_t SLP_EN;
uint16_t SCI_EN;
uint8_t PM1_CNT_LEN;

static uint8_t checksum(acpi_header_t* header) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i < header->length; i++)
        sum += ((char*) header)[i];
    return sum;
}

static void acpi_parse_fadt(fadt_t* fadt) {
    acpi_header_t* dsdt = (acpi_header_t*) fadt->dsdt_address;

    if (checksum(dsdt)) {
        klog(LOG_ERR, "ACPI: %s checksum failed\n", &dsdt->signature);
        return;
    }

    char* S5_addr= (char*) dsdt + sizeof(acpi_header_t);
    uint32_t dsdt_length = dsdt->length - sizeof(acpi_header_t);
    while (0 < dsdt_length--) {
        if (memcmp(S5_addr, "_S5_", 4) == 0)
            break;
        S5_addr++;
    }

    if (dsdt_length > 0) {
        /* check AML structure; what sick fuck came up with this??? */
        S5_addr += 5;
        S5_addr += ((*S5_addr & 0xc0) >> 6) + 2;

        if (*S5_addr == 0x0a)
            S5_addr++;

        SMI_CMD = fadt->smi_cmd;

        ACPI_ENABLE = fadt->acpi_enable;
        ACPI_DISABLE = fadt->acpi_disable;

        PM1a_CNT = fadt->pm1a_cnt_blk;
        PM1b_CNT = fadt->pm1b_cnt_blk;

        PM1_CNT_LEN = fadt->pm1_cnt_length;

        SLP_EN = 1<<13;
        SCI_EN = 1;
    } else {
        klog(LOG_WARN, "ACPI: _S5 not found, some functions will not be available\n");
    }
}

static void acpi_parse_header(acpi_header_t* header) {
    uint32_t signature = header->signature;

    if (checksum(header)) {
        klog(LOG_ERR, "ACPI: %s checksum failed\n", &signature);
        return;
    }

    char sig_string[5];
    memcpy(sig_string, &signature, 4);
    sig_string[4] = 0;

    switch (signature) {
        case 0x50434146: /* FACP */
            acpi_parse_fadt((fadt_t*) header + 0xc0000000);
            break;
    }
}

static void acpi_parse_rsdt(rsdt_t* rsdt) {
    if (checksum((acpi_header_t*) rsdt)) {
        klog(LOG_ERR, "ACPI: RSDT checksum failed\n");
        return;
    }

    for (uint32_t i = 0; i < ((rsdt->header.length - sizeof(acpi_header_t)) / 4); i++) {
        acpi_header_t* header = (acpi_header_t*) rsdt->sdt_entries[i];
        acpi_parse_header(header + 0xc0000000);
    }
}

static void acpi_parse_xsdt(xsdt_t* xsdt) {
    if (checksum((acpi_header_t*) xsdt)) {
        klog(LOG_ERR, "ACPI: XSDT checksum failed\n");
        return;
    }

    for (uint32_t i = 0; i < ((xsdt->header.length - sizeof(acpi_header_t)) / 4); i++) {
        acpi_header_t* header = (acpi_header_t*) ((uint32_t) xsdt->sdt_entries[i]);
        acpi_parse_header(header);
    }
}

static int acpi_enable(void) {
    if (!(inw(PM1a_CNT) & SCI_EN)) {
        if (SMI_CMD && ACPI_ENABLE) {
            outb(SMI_CMD, ACPI_ENABLE);

            int i;
            for (i = 0; i < 300; i++) {
                if ((inw(PM1a_CNT) & SCI_EN) == 1 )
                    break;
                sleep(10);
            }

            if (PM1b_CNT != 0) {
                for (i = 0; i < 300; i++) {
                    if ((inw(PM1b_CNT) & SCI_EN))
                        break;
                    sleep(10);
                }
            }
            if (i < 300)
                return 0;
            else
                return -1;
        } else {
            return -1;
        }
    } else {
        return 0;
    }
}

void acpi_init(uint8_t* rsdp) {
    klog(LOG_OK, "Parsing ACPI tables\n");

    uint8_t sum = 0;
    for (uint8_t i = 0; i < 20; ++i)
        sum += rsdp[i];

    if (sum) {
        klog(LOG_ERR, "ACPI: RSDP checksum verification failed\n");
        return;
    }

    uint8_t revision = rsdp[15];
    if (revision == 0) {
        uint32_t rsdt_address = *(uint32_t*) (rsdp + 16);
        acpi_parse_rsdt((rsdt_t*) rsdt_address + 0xc0000000);
    } else if(revision == 2) {
        uint32_t rsdt_address = *(uint32_t*) (rsdp + 16);
        acpi_parse_rsdt((rsdt_t*) rsdt_address + 0xc0000000);
    } else {
        klog(LOG_ERR, "Unsuppored ACPI version %d\n", revision);
    }

    if (acpi_enable())
        klog(LOG_WARN, "ACPI: enabling ACPI mode failed\n");
}
