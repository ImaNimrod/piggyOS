#ifndef _KERNEL_DEVFS_H
#define _KERNEL_DEVFS_H

#include <display.h>
#include <drivers/rtc.h>
#include <dsa/list.h>
#include <fs/fs.h>
#include <memory/kheap.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* function declarations */
void devfs_init(void);
void devfs_register(fs_node_t* device);

extern void null_device_create(void);
extern void port_device_create(void);

#endif
