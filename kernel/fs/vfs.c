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

    klog(LOG_WARN, "%s: node passed is not a directory\n node = %d name = %s\n", __func__, node->inode, name);
    return NULL;
}

int create_file_fs(char* name) {
    fs_node_t* parent;
    char* cwd = (char*) PATH_SEPARATOR;
    char* path = canonicalize_path(cwd, name);

    char* parent_path = kmalloc(strlen(path) + 5);

    char* f_path = path + strlen(path) - 1;
    while (f_path > path) {
        if (*f_path == '/') {
            f_path += 1;
            break;
        }
        f_path--;
    }

    while (*f_path == '/') {
        f_path++;
    }

    parent = kopen(parent_path, 0);
    kfree(parent_path);

    if (!parent) {
        klog(LOG_WARN, "%s: failed to open parent\n", __func__);
        kfree(path);
        return 1;
    }

    int ret = 0;
    if (parent->create) {
        ret = parent->create(parent, f_path);
    } else {
        ret = 1;
    }

    kfree(path);
    kfree(parent);

    return ret;
}

int mkdir_fs(char* name) {
    fs_node_t* parent;
    char* cwd = (char*) PATH_SEPARATOR;
    char* path = canonicalize_path(cwd, name);

    if (!name || !strlen(name)) {
        return 1;
    }

    char* parent_path = kmalloc(strlen(path) + 5);

    char* f_path = path + strlen(path) - 1;
    while (f_path > path) {
        if (*f_path == '/') {
            f_path += 1;
            break;
        }
        f_path--;
    }

    while (*f_path == '/') {
        f_path++;
    }

    parent = kopen(parent_path, 0);
    kfree(parent_path);

    if (!parent) {
        klog(LOG_WARN, "%s: failed to open parent\n", __func__);
        kfree(path);
        return 1;
    }

    if (!f_path || !strlen(f_path)) {
        return 1;
    }

    int ret = 0;
    if (parent->mkdir) {
        ret = parent->mkdir(parent, f_path);
    } else {
        ret = 1;
    }

    kfree(path);
    close_fs(parent);

    return ret;
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

fs_node_t *kopen_recur(const char *filename, uint32_t flags, uint32_t symlink_depth, char *relative_to) {
	if (!filename) {
		return NULL;
	}

	char* path = canonicalize_path(relative_to, filename);
	size_t path_len = strlen(path);

	if (path_len == 1) {
		fs_node_t* root_clone = kmalloc(sizeof(fs_node_t));
		memcpy(root_clone, fs_root, sizeof(fs_node_t));
		kfree(path);
		open_fs(root_clone, flags);
		return root_clone;
	}

	char* path_offset = path;
	uint64_t path_depth = 0;
	while (path_offset < path + path_len) {
		if (*path_offset == PATH_SEPARATOR) {
			*path_offset = '\0';
			path_depth++;
		}
		path_offset++;
	}
	path[path_len] = '\0';
	path_offset = path + 1;

	uint32_t depth = 0;

	fs_node_t* node_ptr = get_mount_point(path, path_depth, &path_offset, &depth);

	if (!node_ptr) return NULL;

	do {
		if (path_offset >= path+path_len) {
			kfree(path);
			open_fs(node_ptr, flags);
			return node_ptr;
		}

		if (depth == path_depth) {
			open_fs(node_ptr, flags);
			kfree((void*) path);
			return node_ptr;
		}

		fs_node_t* node_next = finddir_fs(node_ptr, path_offset);
		kfree(node_ptr); /* Always a clone or an unopened thing */
		node_ptr = node_next;
		if (!node_ptr) {
			kfree((void*)path);
			return NULL;
		}

		path_offset += strlen(path_offset) + 1;
		depth++;
	} while (depth < path_depth + 1);

	kfree((void*) path);
	return NULL;
}

fs_node_t *kopen(const char* filename, uint32_t flags) {
    kprintf("%s: %s\n", __func__, filename);
	return kopen_recur(filename, flags, 0, (char *) PATH_SEPARATOR);
}

void* vfs_mount(const char* path, fs_node_t* local_root) {
	if (!fs_tree) {
        klog(LOG_ERR, "%s: vfs structure not initalized\n", __func__);
		return NULL;
	}

	if (!path || path[0] != '/') {
        klog(LOG_ERR, "%s: absolute path required\n", __func__);
		return NULL;
	}

	tree_node_t* ret_val = NULL;

	char* p = strdup(path);
	char* i = p;

	int path_len = strlen(p);

	while (i < p + path_len) {
		if (*i == PATH_SEPARATOR) {
			*i = '\0';
		}
		i++;
	}
	p[path_len] = '\0';
	i = p + 1;

	tree_node_t* root_node = fs_tree->root;

	if (*i == '\0') {
		struct vfs_entry* root = (struct vfs_entry*) root_node->value;
		if (root->file) {
            klog(LOG_WARN, "%s: path %s already mounted\n", __func__, path);
		}
		root->file = local_root;

		fs_root = local_root;
		ret_val = root_node;
	} else {
		tree_node_t *node = root_node;
		char* at = i;
		while (1) {
			if (at >= p + path_len)
				break;

			int found = 0;
			foreach(child, node->children) {
				tree_node_t* tchild = (tree_node_t*) child->value;
				struct vfs_entry* entry = (struct vfs_entry*) tchild->value;
				if (!strcmp(entry->name, at)) {
					found = 1;
					node = tchild;
					ret_val = node;
					break;
				}
			}

			if (!found) {
				struct vfs_entry *entry = kmalloc(sizeof(struct vfs_entry));
				entry->name = strdup(at);
				entry->file = NULL;
				entry->device = NULL;
				entry->fs_type = NULL;
				node = tree_node_insert_child(fs_tree, node, entry);
			}
			at = at + strlen(at) + 1;
		}

		struct vfs_entry *entry = (struct vfs_entry *) node->value;
		if (entry->file) {
            klog(LOG_WARN, "%s: path %s already mounted\n", __func__, path);
		}
		entry->file = local_root;
		ret_val = node;
	}

	kfree(p);
	return ret_val;
}

fs_node_t* get_mount_point(char* path, uint32_t path_depth, char** outpath, uint32_t* outdepth) {
	size_t depth;

	for (depth = 0; depth <= path_depth; ++depth) {
		path += strlen(path) + 1;
	}

	fs_node_t* last = fs_root;
	tree_node_t* node = fs_tree->root;

	char* at = *outpath;
	int _depth = 1;
	int _tree_depth = 0;

	while (1) {
		if (at >= path)
			break;

		int found = 0;
		foreach(child, node->children) {
			tree_node_t *tchild = (tree_node_t*) child->value;
			struct vfs_entry *entry = (struct vfs_entry*) tchild->value;

			if (!strcmp(entry->name, at)) {
				found = 1;
				node = tchild;
				at = at + strlen(at) + 1;
				if (entry->file) {
					_tree_depth = _depth;
					last = entry->file;
					*outpath = at;
				}
				break;
			}
		}
		if (!found) {
			break;
		}
		_depth++;
	}

	*outdepth = _tree_depth;

	if (last) {
		fs_node_t * last_clone = kmalloc(sizeof(fs_node_t));
		memcpy(last_clone, last, sizeof(fs_node_t));
		return last_clone;
	}

	return last;
}

fs_node_t* vfs_get_root(void) {
    return fs_root;
}

void vfs_init(void) {
    klog(LOG_OK, "Initializing VFS\n");
	fs_tree = tree_create();

	struct vfs_entry* root = kmalloc(sizeof(struct vfs_entry));

	root->name = strdup("[root]");
	root->file = NULL; 
	root->fs_type = NULL;
	root->device = NULL;

	tree_set_root(fs_tree, root);
}
