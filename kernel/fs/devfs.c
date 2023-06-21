#include <fs/devfs.h>

static list_t* devlist;
static struct dirent* devfs_dirent;
static spinlock_t devfs_lock;

static struct dirent* devfs_readdir(fs_node_t* node, uint32_t index) {
    struct dirent* result = NULL;
    uint32_t counter = 0;
    spin_lock(&devfs_lock);

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

    spin_unlock(&devfs_lock);
    return result;
}

static fs_node_t* devfs_finddir(fs_node_t* node, char* name) {
    fs_node_t* result = NULL;

    spin_lock(&devfs_lock);

    foreach(d, devlist) {
        fs_node_t* dev_node = (fs_node_t*) d->value;

        if (!strcmp(name, dev_node->name)) {
            result = dev_node;
            break;
        }
    }

    spin_unlock(&devfs_lock);
    return result;
}

void devfs_register(fs_node_t* device) {
    spin_lock(&devfs_lock);

    foreach(d, devlist) {
        fs_node_t* devfs_node = (fs_node_t*) d->value;

        if (!strcmp(device->name, devfs_node->name)) {
            klog(LOG_ERR, "Could not create fs_node: %s, node by that name already exists\n");
            spin_unlock(&devfs_lock);
            return;
        }
    }

    list_insert_front(devlist, device);
    spin_unlock(&devfs_lock);
}

void devfs_init(void) {
    klog(LOG_OK, "Initializing devFS\n");
    fs_node_t* devfs = (fs_node_t*) kmalloc(sizeof(fs_node_t));

    strcpy(devfs->name, "dev");
    devfs->flags = FS_DIRECTORY | FS_MOUNTPOINT;
    devfs->inode = 0;

    devfs->atime = get_seconds();
    devfs->ctime = devfs->atime;

    devfs->open = NULL;
    devfs->close = NULL;
    devfs->read = NULL;
    devfs->write = NULL;
    devfs->readdir = &devfs_readdir;
    devfs->finddir = &devfs_finddir;
    devfs->ioctl = NULL;

    fs_register(devfs);

    devlist = list_create();

    com_device_create(PORT_COM1);
    null_device_create();
    port_device_create();
    random_device_create();
}
