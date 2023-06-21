#include <fs/devfs.h>

static size_t read_null(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf) {
    (void) node;
    (void) offset;
    (void) size;
    (void) buf;
	return 0;
}

static size_t write_null(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf) {
    (void) node;
    (void) offset;
    (void) buf;
    return size;
}

void null_device_create(void) {
    fs_node_t* null = (fs_node_t*) kmalloc(sizeof(fs_node_t));
    
	strcpy(null->name, "null");
	null->flags = FS_CHARDEVICE;
    null->inode = 0;

    null->atime = get_seconds();
    null->ctime = null->atime;

    null->open = NULL;
    null->close = NULL;
    null->read = &read_null;
    null->write = &write_null;
    null->ioctl = NULL;

    devfs_register(null);
}
