#include <drivers/dma.h>

static void dma_set_mode (uint8_t channel, uint8_t mode) {
    uint16_t dma = (channel < 4) ? 0 : 1;
    uint16_t chan = (dma==0) ? channel : channel-4;

    dma_mask_channel (channel);
    outb ( (channel < 4) ? (DMA0_MODE_REG) : DMA1_MODE_REG, chan | (mode) );
    dma_unmask_all();
}

void dma_mask_channel(uint8_t channel){
    if (channel <= 4) {
        outb(DMA0_CHANMASK_REG, (1 << (channel-1)));
    } else {
        outb(DMA1_CHANMASK_REG, (1 << (channel-5)));
    }
}

void dma_unmask_channel(uint8_t channel) {
    if (channel <= 4) {
        outb(DMA0_CHANMASK_REG, channel);
    } else {
        outb(DMA1_CHANMASK_REG, channel);
    }
}

void dma_unmask_all(void){
    outb(DMA1_UNMASK_ALL_REG, 0xff);
}

void dma_reset(void) {
    outb(DMA0_TEMP_REG, 0xff);
}

void dma_reset_flipflop(int dma) {
    if (dma < 2)
        return;
    outb((dma==0) ? DMA0_CLEARBYTE_FLIPFLOP_REG : DMA1_CLEARBYTE_FLIPFLOP_REG, 0xff);
}

void dma_set_address(uint8_t channel, uint8_t low, uint8_t high) {
    if (channel > 8 )
        return;

    uint16_t port = 0;
    switch (channel) {
        case 0: {port = DMA0_CHAN0_ADDR_REG; break;}
        case 1: {port = DMA0_CHAN1_ADDR_REG; break;}
        case 2: {port = DMA0_CHAN2_ADDR_REG; break;}
        case 3: {port = DMA0_CHAN3_ADDR_REG; break;}
        case 4: {port = DMA1_CHAN4_ADDR_REG; break;}
        case 5: {port = DMA1_CHAN5_ADDR_REG; break;}
        case 6: {port = DMA1_CHAN6_ADDR_REG; break;}
        case 7: {port = DMA1_CHAN7_ADDR_REG; break;}
    }
    outb(port, low);
    outb(port, high);
}

void dma_set_count(uint8_t channel, uint8_t low, uint8_t high) {
    if (channel > 8)
        return;

    uint16_t port = 0;
    switch (channel) {
        case 0: {port = DMA0_CHAN0_COUNT_REG; break;}
        case 1: {port = DMA0_CHAN1_COUNT_REG; break;}
        case 2: {port = DMA0_CHAN2_COUNT_REG; break;}
        case 3: {port = DMA0_CHAN3_COUNT_REG; break;}
        case 4: {port = DMA1_CHAN4_COUNT_REG; break;}
        case 5: {port = DMA1_CHAN5_COUNT_REG; break;}
        case 6: {port = DMA1_CHAN6_COUNT_REG; break;}
        case 7: {port = DMA1_CHAN7_COUNT_REG; break;}
    }
    outb(port, low);
    outb(port, high);
}

void dma_set_read (uint8_t channel) {
    dma_set_mode(channel, DMA_MODE_READ_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}

void dma_set_write (uint8_t channel) {
    dma_set_mode(channel, DMA_MODE_WRITE_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}
