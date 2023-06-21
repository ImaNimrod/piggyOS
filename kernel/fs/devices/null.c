#include <fs/fs.h>

static uint32_t read_null(fs_node_t *node, uint32_t offset, size_t size, uint8_t *buf);
static uint32_t write_null(fs_node_t *node, uint32_t offset, size_t size, uint8_t *buf);
static void open_null(fs_node_t *node, uint32_t flags);
static void close_null(fs_node_t *node);

static uint32_t read_null(fs_node_t *node, uint32_t offset, size_t size, uint8_t *buf) {
    (void) node;
    (void) offset;
    (void) size;
    (void) buf;
	return 0;
}

static uint32_t write_null(fs_node_t *node, uint32_t offset, size_t size, uint8_t *buf) {
    (void) node;
    (void) offset;
    (void) buf;
    return size;
}

static void open_null(fs_node_t *node, uint32_t flags) {
    (void) node;
    (void) flags;
	return;
}

static void close_null(fs_node_t *node) {
    (void) node;
	return;
}

extern fs_node_t *null_device_create(void) {
	fs_node_t *fnode = kmalloc(sizeof(fs_node_t));
	memset(fnode, 0x00, sizeof(fs_node_t));
	fnode->inode = 0;
	strcpy(fnode->name, "null");
	fnode->flags   = FS_CHARDEVICE;
	fnode->read    = read_null;
	fnode->write   = write_null;
	fnode->open    = open_null;
	fnode->close   = close_null;
	fnode->readdir = NULL;
	fnode->finddir = NULL;
	return fnode;
}
