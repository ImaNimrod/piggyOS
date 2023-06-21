#include <fs/fs.h>

static uint32_t read_zero(fs_node_t *node, uint32_t offset, size_t size, uint8_t *buf);
static uint32_t write_zero(fs_node_t *node, uint32_t offset, size_t size, uint8_t *buf);
static void open_zero(fs_node_t *node, uint32_t flags);
static void close_zero(fs_node_t *node);

static uint32_t read_zero(fs_node_t *node, uint32_t offset, size_t size, uint8_t *buf) {
    memset(buf, 0x00, size);
    (void) node;
    (void) offset;
	return 1;
}

static uint32_t write_zero(fs_node_t *node, uint32_t offset, size_t size, uint8_t *buf) {
    (void) node;
    (void) offset;
    (void) buf;
    return size;
}

static void open_zero(fs_node_t *node, uint32_t flags) {
    (void) node;
    (void) flags;
	return;
}

static void close_zero(fs_node_t *node) {
    (void) node;
	return;
}

extern fs_node_t *zero_device_create(void) {
	fs_node_t *fnode = kmalloc(sizeof(fs_node_t));
	memset(fnode, 0x00, sizeof(fs_node_t));
	fnode->inode = 0;
	strcpy(fnode->name, "zero");
	fnode->flags   = FS_CHARDEVICE;
	fnode->read    = read_zero;
	fnode->write   = write_zero;
	fnode->open    = open_zero;
	fnode->close   = close_zero;
	fnode->readdir = NULL;
	fnode->finddir = NULL;
	return fnode;
}
