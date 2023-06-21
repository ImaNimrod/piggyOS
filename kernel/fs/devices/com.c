#include <drivers/serial.h>
#include <fs/devfs.h>

static spinlock_t com_lock;

static ssize_t read_com(fs_node_t* node, off_t offset, size_t size, uint8_t* buf) {
    (void) offset;

    for(size_t i = 0; i < size; i++)
        *buf++ = serial_getc();

    return size;
}

static ssize_t write_com(fs_node_t* node, off_t offset, size_t size, uint8_t* buf) {
    (void) offset;

    spin_lock(&com_lock);
    for(size_t i = 0;i < size; i++)
        serial_putc(*buf++);
    spin_unlock(&com_lock);

    return size;
}

void com_device_create(int device) {
    fs_node_t* com = (fs_node_t*) kmalloc(sizeof(fs_node_t));

	strcpy(com->name, "com0");
	com->flags = FS_CHARDEVICE;
    com->inode = 0;

    com->atime = get_seconds();
    com->ctime = com->atime;

    com->open = NULL;
    com->close = NULL;
    com->read = &read_com;
    com->write = &write_com;
    com->ioctl = NULL;

    devfs_register(com);
}
