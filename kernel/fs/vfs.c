#include <fs/fs.h>

tree_t* fs_tree = NULL;
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

/* for all things dealing with paths */
char* canonicalize_path(const char* cwd, const char* input) {
    list_t* out = list_create();

    if (strlen(input) && input[0] != PATH_SEPARATOR) {
        char* path = kmalloc((strlen(cwd) + 1) * sizeof(char));
        memcpy(path, cwd, strlen(cwd) + 1);

        char* pch;
        char* save;
        pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

        while (pch != NULL) {
            char* s = kmalloc(sizeof(char) * (strlen(pch) + 1));
            memcpy(s, pch, strlen(pch) + 1);
            list_insert_front(out, s);
            pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
        }

        kfree(path);
    }

    char* path = kmalloc((strlen(input) + 1) * sizeof(char));
    memcpy(path, input, strlen(input) + 1);

    char* pch;
    char* save;
    pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

    while (pch != NULL) {
        if (!strcmp(pch,PATH_UP)) {
            list_node_t* n = list_pop(out);
            if (n) {
                kfree(n->value);
                kfree(n);
            }
        } else if (!strcmp(pch,PATH_DOT)) {
            break;
        } else {
            char* s = kmalloc(sizeof(char) * (strlen(pch) + 1));
            memcpy(s, pch, strlen(pch) + 1);
            list_insert_front(out, s);
        }
        pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
    }
    kfree(path);

    size_t size = 0;
    foreach(item, out) {
        size += strlen(item->value) + 1;
    }

    char* output = kmalloc(sizeof(char) * (size + 1));
    char* output_offset = output;
    if (size == 0) {
        output = kmalloc(sizeof(char) * 2);
        output[0] = PATH_SEPARATOR;
        output[1] = '\0';
    } else {
        foreach(item, out) {
            output_offset[0] = PATH_SEPARATOR;
            output_offset++;
            memcpy(output_offset, item->value, strlen(item->value) + 1);
            output_offset += strlen(item->value);
        }
    }

    foreach(item, out) {
        listnode_destroy(item);
    }
    list_destroy(out);
    kfree(out);

    return output;
}
