#include <fs/devfs.h>

static list_t* devlist;
static spinlock_t devlist_lock;
static struct dirent* devfs_dirent;

static void open_devfs(fs_node_t* node, uint32_t flags) {
    (void) node;
    (void) flags;
	return;
}

static struct dirent* devfs_readdir(fs_node_t* node, uint32_t index) {
    struct dirent* result = NULL;
    uint32_t counter = 0;

    spin_lock(&devlist_lock);

    foreach(d, devlist) {
        if (index == counter) {
            fs_node_t* device_node = (fs_node_t*) d->value;
            strcpy(devfs_dirent->name, device_node->name);
            devfs_dirent->inode = index;
            result = devfs_dirent;
            break;
        }
        counter++;
    }

    spin_unlock(&devlist_lock);
    return result;
}

static fs_node_t* devfs_finddir(fs_node_t* node, char* name) {
    fs_node_t* result = NULL;
    spin_lock(&devlist_lock);

    foreach(d, devlist) {
        fs_node_t* dev_node = (fs_node_t*) d->value;

        if (strcmp(name, dev_node->name) == 0) {
            result = dev_node;
            break;
        }
    }

    spin_unlock(&devlist_lock);
    return result;
}

void devfs_init(void) {
    klog(LOG_OK, "Initializing devFS\n");
    fs_node_t* devfs = kcalloc(sizeof(fs_node_t), 1);

    devfs->flags = FS_DIRECTORY;

    devfs->open = open_devfs;
    devfs->close = NULL;
    devfs->finddir = devfs_finddir;
    devfs->readdir = devfs_readdir;

    devlist = list_create();
    spinlock_init(&devlist_lock);

    vfs_mount("/dev", devfs);
}

fs_node_t* devfs_register(device_t* device) {
    spin_lock(&devlist_lock);

    foreach(d, devlist) {
        fs_node_t* dev_node = (fs_node_t*) d->value;

        if (strcmp(device->name, dev_node->name) == 0) {
            spin_unlock(&devlist_lock);
            return NULL;
        }
    }

    fs_node_t* dev_node = (fs_node_t*) kcalloc(sizeof(fs_node_t), 1);
    strcpy(dev_node->name, device->name);
    dev_node->flags = device->type;
    dev_node->open = device->open;
    dev_node->close = device->close;
    dev_node->read = device->read;
    dev_node->write = device->write;
    dev_node->ioctl = device->ioctl;

    list_insert_front(devlist, dev_node);
    spin_unlock(&devlist_lock);

    return dev_node;
}
