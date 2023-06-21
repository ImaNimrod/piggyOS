#include <fs/fs.h>

fs_node_t* fs_root = NULL;
tree_t* fs_tree = NULL;

/* file operations */
void open_fs(fs_node_t* node, uint32_t flags) {
    if (!node) return;

    if (node->open)
        node->open(node, flags);
}

void close_fs(fs_node_t* node) {
    if (!node) return;
    if (node == fs_root) return;

    if (node->close)
        node->close(node);

    kfree(node);
}

ssize_t read_fs(fs_node_t* node, off_t offset, size_t size, uint8_t* buffer) {
	if (!node) return 0;

	if (node->read) 
		return node->read(node, offset, size, buffer);

    return 0;
}

ssize_t write_fs(fs_node_t* node, off_t offset, size_t size, uint8_t* buffer) {
	if (!node) return 0;

	if (node->write)
		return node->write(node, offset, size, buffer);

    return 0;
}

struct dirent* readdir_fs(fs_node_t* node, uint32_t index) {
	if (!node) return NULL;

	if ((node->flags & FS_DIRECTORY) && node->readdir)
		return node->readdir(node, index);

    return NULL;
}

fs_node_t* finddir_fs(fs_node_t* node, char* name) {
	if (!node) return NULL;

	if ((node->flags & FS_DIRECTORY) && node->finddir)
		return node->finddir(node, name);

    klog(LOG_WARN, "%s: node passed is not a directory\n\t    node = %d name = %s\n", __func__, node->inode, name);
    return NULL;
}

int ioctl_fs(fs_node_t* node, uint32_t request, void* argp) {
	if (!node) return 0;

	if (node->ioctl)
		return node->ioctl(node, request, argp);
    return 0;
}

fs_node_t* clone_fs(fs_node_t *node) {
	if (!node) return NULL;

	fs_node_t* n = (fs_node_t*) kmalloc(sizeof(fs_node_t));
	memcpy(n, node, sizeof(fs_node_t));
	return n;
}

void vfs_init(void) {
    klog(LOG_OK, "Initializing VFS structures\n");
    fs_root = rootfs_init();
}

fs_node_t* get_fs_root(void) {
    return fs_root;
}
