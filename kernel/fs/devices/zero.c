#include <fs/devfs.h>

static size_t read_zero(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf) {
    memset(buf, 0x00, size);
    (void) node;
    (void) offset;
	return 1;
}

static size_t write_zero(fs_node_t *node, uint32_t offset, size_t size, uint8_t* buf) {
    (void) node;
    (void) offset;
    (void) buf;
    return size;
}

static void open_zero(fs_node_t* node, uint32_t flags) {
    (void) node;
    (void) flags;
	return;
}

static void close_zero(fs_node_t* node) {
    (void) node;
	return;
}

void zero_device_create(void) {
    device_t zero;

	strcpy(zero.name, "zero");
	zero.type = FS_CHARDEVICE;
    zero.open = open_zero;
    zero.close = close_zero;
    zero.read = read_zero;
    zero.write = write_zero;
    zero.ioctl = NULL;

    devfs_register(&zero);
}
