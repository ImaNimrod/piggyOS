#ifndef _KERNEL_DEVFS_H
#define _KERNEL_DEVFS_H

#include <display.h>
#include <fs/fs.h>
#include <list.h>
#include <memory/kheap.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/spinlock.h>

typedef struct {
    char name[32];
    uint32_t type;
    open_type_t open;
    close_type_t close;
    read_type_t read;   
    write_type_t write;
    ioctl_type_t ioctl;
} device_t;


/* function declarations */
void devfs_init(void);
fs_node_t* devfs_register(device_t* device);

extern void null_device_create(void);
extern void port_device_create(void);
extern void zero_device_create(void);

#endif
