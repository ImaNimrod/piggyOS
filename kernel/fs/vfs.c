#include <fs/fs.h>

fs_node_t* fs_root = NULL;

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

size_t read_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf) {
	if (!node) return 0;

	if (node->read) 
		return node->read(node, offset, size, buf);

    return 0;
}

size_t write_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf) {
	if (!node) return 0;

	if (node->write)
		return node->write(node, offset, size, buf);

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

char** path(char* file) {
    uint32_t i = 0, i0 = 0, seg = 0, segs = 0;
    char** p;
    char* str;

    if (!file)
        return NULL;

    if(file[i] == '/')
        i0++;

    str = strdup(file);
    str = &str[i0]; // skip initial '/'
    segs = 1;

    while(str[i]) {
        if(str[i] == '/') {
            str[i] = '\0';

            if(str[i+1] == '\0')
                break;
            segs++;
        }
        i++;
    }

    p = (char**) kmalloc(sizeof(char*) * (segs));

    for(seg = 0, i = 0; seg < segs; seg++) {
        p[seg] = &str[i];
        while(str[i++]);
    }

    p[seg] = NULL;

    return p;
}

void path_free(char** p) {
    kfree(p[0]);
}

void vfs_init(void) {
    klog(LOG_OK, "Initializing VFS structures\n");
    fs_root = rootfs_init();
}

fs_node_t* get_fs_root(void) {
    return fs_root;
}
