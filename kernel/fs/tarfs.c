#include <fs/tarfs.h>

static struct tar_header* headers[NUM_TARFILES];
static fs_node_t* tarfs_root;
static uint32_t tarfs_files;

static uint32_t tar_getsize(const char* in) {
    uint32_t size = 0;
    uint32_t count = 1;
 
    for (uint32_t j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);
 
    return size;
 
}

static void tar_parse(uint32_t address, uint32_t end) {
        for (tarfs_files = 0; ; tarfs_files++) {

        struct tar_header* header = (struct tar_header*) address;

        if (header->name[0] == '\0')
            break;

        uint32_t size = tar_getsize(header->size);

        headers[tarfs_files] = header;

        address += ((size / 512) + 1) * 512;
        if (size % 512)
            address += 512;

        if(address > end)
          break;
    }
}

static ssize_t tarfs_read(fs_node_t* node, off_t offset, size_t size, uint8_t* buffer) {
    uint32_t file_sz = node->length;

    if ((size_t) offset > file_sz)
        return 0;

    if (offset + size > file_sz)
        size = file_sz - offset;

    memcpy(buffer, (uint8_t*) ((uint32_t) headers[node->inode] + 512), size);

    return size;
}

struct dirent* tarfs_readdir(fs_node_t* node, uint32_t index) {
    struct dirent* d;
    fs_node_t* i_node;

    list_t* nodes_list = (list_t*) node->inode;

    list_node_t* entry = list_get_node_by_index(nodes_list, index);

    if (!entry)
        return NULL;
    
    i_node = (fs_node_t*) entry->value;

    d = (struct dirent*) kmalloc(sizeof(struct dirent));

    if(d == NULL)
        return NULL;

    strcpy(d->name, i_node->name);
    d->inode = i_node->inode;
    d->flags = i_node->flags;
    d->offset = index;

    return d;
}

fs_node_t* tarfs_finddir(fs_node_t* node, char* name) {
    if (!name)
        return NULL;

    list_t* nodes_list = (list_t*) node->inode;

    foreach(d, nodes_list) {
        fs_node_t* tarfs_node = (fs_node_t*) d->value;

        if (!strcmp(name, tarfs_node->name))
            return tarfs_node;
    }

    return NULL;
}

void tarfs_init(uint32_t start, uint32_t end) {
    klog(LOG_OK, "Mounting ramdisk at /initrd\n");

    tar_parse(start, end);
    tarfs_root = (fs_node_t*) kmalloc(sizeof(fs_node_t));  

    strcpy(tarfs_root->name, "initrd");
    tarfs_root->flags = FS_DIRECTORY;
    tarfs_root->inode = (uint32_t) list_create();

    tarfs_root->atime = get_seconds();
    tarfs_root->ctime = tarfs_root->atime;

    tarfs_root->open = NULL;
    tarfs_root->close = NULL;
    tarfs_root->read = NULL;
    tarfs_root->write = NULL;
    tarfs_root->readdir = &tarfs_readdir;
    tarfs_root->finddir = &tarfs_finddir;
    tarfs_root->ioctl = NULL;

    fs_node_t* node = NULL;
    for (uint32_t i = 1; i < tarfs_files; i++) {
        switch (headers[i]->typeflag) {
            case AREGTYPE: break;
            case REGTYPE:
                node = (fs_node_t*) kmalloc(sizeof(fs_node_t));

                strcpy(node->name, headers[i]->name);
                node->flags = FS_FILE;
                node->inode = i;
                node->length = tar_getsize(headers[i]->size);

                node->atime = get_seconds();
                node->ctime = node->atime;

                node->open = NULL;
                node->close = NULL;
                node->read = &tarfs_read;
                node->write = NULL;
                node->readdir = NULL;
                node->finddir = NULL;
                node->ioctl = NULL;
                break;
            default:
                kprintf("tarfs: '%s' filetype (%c) not suported\n", headers[i]->name, headers[i]->typeflag);
                break;
        }   
        list_insert_front((list_t*) tarfs_root->inode, node);
    }

    fs_register(tarfs_root);
}
