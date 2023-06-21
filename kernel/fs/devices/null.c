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

static void open_null(fs_node_t* node, uint32_t flags) {
    (void) node;
    (void) flags;
	return;
}

static void close_null(fs_node_t* node) {
    (void) node;
	return;
}

void null_device_create(void) {
    device_t null;

	strcpy(null.name, "null");
	null.type = FS_CHARDEVICE;
    null.open = open_null;
    null.close = close_null;
    null.read = read_null;
    null.write = write_null;
    null.ioctl = NULL;

    devfs_register(&null);
}
