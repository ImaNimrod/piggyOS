#include <fs/fs.h>

static list_t* root_nodes;
static struct dirent* rootfs_dirent;
static spinlock_t rootfs_lock = 1;

static struct dirent* rootfs_readdir(fs_node_t* node, uint32_t index) {
    struct dirent* result = NULL;
    uint32_t counter = 0;
    spin_lock(&rootfs_lock);

    foreach(d, root_nodes) {
        if (index == counter) {
            fs_node_t* rootfs_node = (fs_node_t*) d->value;
            strcpy(rootfs_dirent->name, rootfs_node->name);
            rootfs_dirent->inode = index;
            result = rootfs_dirent;
            break;
        }
        counter++;
    }

    spin_unlock(&rootfs_lock);
    return result;
}

static fs_node_t* rootfs_finddir(fs_node_t* node, char* name) {
    fs_node_t* result = NULL;

    spin_lock(&rootfs_lock);
    foreach(d, root_nodes) {
        fs_node_t* rootfs_node = (fs_node_t*) d->value;

        if (strcmp(name, rootfs_node->name) == 0) {
            result = rootfs_node;
            break;
        }
    }

    spin_unlock(&rootfs_lock);
    return result;
}

void fs_register(fs_node_t* node) {
    spin_lock(&rootfs_lock);

    foreach(d, root_nodes) {
        fs_node_t* rootfs_node = (fs_node_t*) d->value;

        if (!strcmp(node->name, rootfs_node->name)) {
            klog(LOG_ERR, "Could not create fs_node: %s, node by that name already exists\n");
            spin_unlock(&rootfs_lock);
            return;
        }
    }

    list_insert_front(root_nodes, node);
    spin_unlock(&rootfs_lock);
}

fs_node_t* rootfs_init(void) {
    fs_node_t* rootfs = (fs_node_t*) kcalloc(sizeof(fs_node_t), 1);

    strcpy(rootfs->name, "/");
    rootfs->flags = FS_DIRECTORY;

    rootfs->read = 0;
    rootfs->write = 0;
    rootfs->open = 0;
    rootfs->close = 0;
    rootfs->readdir = &rootfs_readdir;
    rootfs->finddir = &rootfs_finddir;

    root_nodes = list_create();

    return rootfs;
}
