#include <fs/fs.h>

static list_t* root_nodes;
static spinlock_t rootfs_lock;
static uint32_t root_files;

static struct dirent* rootfs_readdir(fs_node_t* node, uint32_t index) {
    struct dirent* d = NULL;

    fs_node_t* rnode;
    list_node_t* entry;

    if(index >= root_files)
        return NULL;

    entry = list_get_node_by_index(root_nodes, index);
    rnode = (fs_node_t*) entry->value;

    if (rnode) {
        d = (struct dirent*) kmalloc(sizeof(struct dirent));

        if(!d)
            return NULL;

        strcpy(d->name, rnode->name);
        d->inode = (uint32_t) rnode;
        d->offset = index;
        d->flags = rnode->flags;
    }

    spin_unlock(&rootfs_lock);
    return d;
}

static fs_node_t* rootfs_finddir(fs_node_t* node, char* name) {
    fs_node_t* result = NULL;

    spin_lock(&rootfs_lock);
    foreach(d, root_nodes) {
        fs_node_t* rootfs_node = (fs_node_t*) d->value;

        if (!strcmp(name, rootfs_node->name)) {
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
    root_files++;
    spin_unlock(&rootfs_lock);
}

fs_node_t* rootfs_init(void) {
    fs_node_t* rootfs = (fs_node_t*) kcalloc(sizeof(fs_node_t), 1);

    strcpy(rootfs->name, "/");
    rootfs->flags = FS_DIRECTORY | FS_MOUNTPOINT;

    rootfs->open = NULL;
    rootfs->close = NULL;
    rootfs->read = NULL;
    rootfs->write = NULL;
    rootfs->readdir = &rootfs_readdir;
    rootfs->finddir = &rootfs_finddir;
    rootfs->ioctl = NULL;

    root_nodes = list_create();

    return rootfs;
}
