#ifndef _KERNEL_TARFS_H
#define _KERNEL_TARFS_H

#include <display.h>
#include <drivers/rtc.h>
#include <dsa/list.h>
#include <fs/fs.h>
#include <memory/kheap.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <types.h>

#define NUM_TARFILES 64

#define REGTYPE  '0'    /* regular file */
#define AREGTYPE '\0'   /* regular file */
#define LNKTYPE  '1'    /* link */
#define SYMTYPE  '2'    /* reserved */
#define CHRTYPE  '3'    /* character special */
#define BLKTYPE  '4'    /* block special */
#define DIRTYPE  '5'    /* directory */
#define FIFOTYPE '6'    /* FIFO special */
#define CONTTYPE '7'    /* reserved */

struct tar_header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char signature[6];
    char version[2];
    char user_name[32];
    char group_name[32];
    char device_major_number[8];
    char device_minor_number[8];
    char fprefix[155];
    char reserved[12];
};

/* function declarations */
void tarfs_init(uint32_t start, uint32_t end);

#endif
