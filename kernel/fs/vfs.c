#include <fs/fs.h>

// tree_t *fs_tree = NULL;
fs_node_t *fs_root = NULL;
// hashmap_t *fs_types = NULL;

/* 6 basic functions god bless */
uint32_t read_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf) {
	if (!node) return 0;
	if (node->read) {
		return node->read(node, offset, size, buf);
	} else {
		return 0;
	}
}

uint32_t write_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buf) {
	if (!node) return 0;
	if (node->write) {
		return node->write(node, offset, size, buf);
	} else {
		return 0;
	}
}

void open_fs(fs_node_t* node, uint32_t flags) {
	if (!node) return;

	if (node->open) {
		node->open(node, flags);
	}
}

void close_fs(fs_node_t* node) {
	if (!node) return;
    if (node == fs_root) return;

    if (node->close) {
        node->close(node);
    }

    kfree(node);
}

struct dirent* readdir_fs(fs_node_t* node, uint32_t index) {
	if (!node) return NULL;

	if ((node->flags & FS_DIRECTORY) && node->readdir) {
		return node->readdir(node, index);
	} else {
		return NULL;
	}
}

fs_node_t* finddir_fs(fs_node_t* node, char* name) {
	if (!node) return NULL;

	if ((node->flags & FS_DIRECTORY) && node->finddir) {
		return node->finddir(node, name);
	} else {
		return NULL;
	}
}
//
// static char *canonicalize_path(const char *cwd, const char *input) {
//     list_t *out = list_create("vfs canonicalize_path working memory", input);
//
//     if (strlen(input) && input[0] != PATH_SEPARATOR) {
//         char *path = kmalloc((strlen(cwd) + 1) * sizeof(char));
//         memcpy(path, cwd, strlen(cwd) + 1);
//
//         char *pch;
//         char *save;
//         pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);
//
//         while (pch != NULL) {
//             char *s = kmalloc(sizeof(char) * (strlen(pch) + 1));
//             memcpy(s, pch, strlen(pch) + 1);
//             list_insert(out, s);
//             pch = strtok_r(NULL,PATH_SEPARATOR_STRING,&save);
//         }
//         kfree(path);
//     }
//
//     char *path = kmalloc((strlen(input) + 1) * sizeof(char));
//     memcpy(path, input, strlen(input) + 1);
//
//     char *pch;
//     char *save;
//     pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);
//
//     while (pch != NULL) {
//         if (!strcmp(pch,PATH_UP)) {
//             node_t * n = list_pop(out);
//             if (n) {
//                 kfree(n->value);
//                 kfree(n);
//             }
//         } else if (!strcmp(pch,PATH_DOT)) {
//         } else {
//             char * s = kmalloc(sizeof(char) * (strlen(pch) + 1));
//             memcpy(s, pch, strlen(pch) + 1);
//             list_insert(out, s);
//         }
//         pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
//     }
//     kfree(path);
//
//     size_t size = 0;
//     foreach(item, out) {
//         size += strlen(item->value) + 1;
//     }
//
//     char *output = kmalloc(sizeof(char) * (size + 1));
//     char *output_offset = output;
//     if (size == 0) {
//         output = kmalloc(sizeof(char) * 2);
//         output[0] = PATH_SEPARATOR;
//         output[1] = '\0';
//     } else {
//         foreach(item, out) {
//             output_offset[0] = PATH_SEPARATOR;
//             output_offset++;
//             memcpy(output_offset, item->value, strlen(item->value) + 1);
//             output_offset += strlen(item->value);
//         }
//     }
//
//     list_destroy(out);
//     list_free(out);
//     kfree(out);
//
//     return output;
// }
//
// static struct dirent *readdir_mapper(fs_node_t *node, uint32_t index) {
// 	tree_node_t *d = (tree_node_t *) node->device;
//
// 	if (!d) return NULL;
//
// 	if (index == 0) {
// 		struct dirent *dir = kmalloc(sizeof(struct dirent));
// 		strcpy(dir->name, ".");
// 		dir->inode = 0;
// 		return dir;
// 	} else if (index == 1) {
// 		struct dirent *dir = kmalloc(sizeof(struct dirent));
// 		strcpy(dir->name, "..");
// 		dir->inode = 1;
// 		return dir;
// 	}
//
// 	index -= 2;
// 	uint32_t i = 0;
// 	foreach(child, d->children) {
// 		if (i == index) {
// 			tree_node_t *tchild = (tree_node_t *) child->value;
// 			struct vfs_entry *n = (struct vfs_entry *) tchild->value;
// 			struct dirent *dir = kmalloc(sizeof(struct dirent));
//
// 			size_t len = strlen(n->name) + 1;
// 			memcpy(&dir->name, n->name, min(256, len));
// 			dir->inode = i;
// 			return dir;
// 		}
// 		++i;
// 	}
//
// 	return NULL;
// }
//
// static fs_node_t *vfs_mapper(void) {
// 	fs_node_t *fnode = kmalloc(sizeof(fs_node_t));
// 	memset(fnode, 0x00, sizeof(fs_node_t));
// 	fnode->flags   = FS_DIRECTORY;
// 	fnode->readdir = readdir_mapper;
// 	return fnode;
// }
//
// /* the only difference between making "files" and "directorys" is the flags they use
//  * these next two funcs could (should) be one...
//  */
// int create_file_fs(char *name) {
// 	fs_node_t *parent;
// 	char *cwd = (char *) PATH_SEPARATOR;
// 	char *path = canonicalize_path(cwd, name);
//
// 	char *parent_path = kmalloc(strlen(path) + 5);
//
// 	char *f_path = path + strlen(path) - 1;
// 	while (f_path > path) {
// 		if (*f_path == '/') {
// 			f_path += 1;
// 			break;
// 		}
// 		f_path--;
// 	}
//
// 	while (*f_path == '/') {
// 		f_path++;
// 	}
//
// 	parent = kopen(parent_path, 0);
// 	kfree(parent_path);
//
// 	if (!parent) {
//         kprintf("%s: failed to open parent\n", __func__);
// 		kfree(path);
// 		return 1;
// 	}
//
// 	int ret = 0;
// 	if (parent->create) {
// 		ret = parent->create(parent, f_path);
// 	} else {
// 		ret = 1;
// 	}
//
// 	kfree(path);
// 	kfree(parent);
//
// 	return ret;
// }
//
// int mkdir_fs(char *name) {
// 	fs_node_t *parent;
// 	char *cwd = (char *) PATH_SEPARATOR;
// 	char *path = canonicalize_path(cwd, name);
//
// 	if (!name || !strlen(name)) {
// 		return 1;
// 	}
//
// 	char *parent_path = kmalloc(strlen(path) + 5);
//
// 	char *f_path = path + strlen(path) - 1;
// 	while (f_path > path) {
// 		if (*f_path == '/') {
// 			f_path += 1;
// 			break;
// 		}
// 		f_path--;
// 	}
//
// 	while (*f_path == '/') {
// 		f_path++;
// 	}
//
// 	parent = kopen(parent_path, 0);
// 	kfree(parent_path);
//
// 	if (!parent) {
//         kprintf("%s: failed to open parent\n", __func__);
// 		kfree(path);
// 		return 1;
// 	}
//
// 	if (!f_path || !strlen(f_path)) {
// 		return 1;
// 	}
//
// 	int ret = 0;
// 	if (parent->mkdir) {
// 		ret = parent->mkdir(parent, f_path);
// 	} else {
// 		ret = 1;
// 	}
//
// 	kfree(path);
// 	close_fs(parent);
//
// 	return ret;
// }
//
// /* easy stuff for symlinks */
// int symlink_fs(char *target, char *name) {
// 	fs_node_t *parent;
// 	char *cwd = (char *) PATH_SEPARATOR;
// 	char *path = canonicalize_path(cwd, name);
//
// 	char *parent_path = kmalloc(strlen(path) + 5);
// 	char *f_path = path + strlen(path) - 1;
// 	while (f_path > path) {
// 		if (*f_path == '/') {
// 			f_path += 1;
// 			break;
// 		}
// 		f_path--;
// 	}
//
// 	parent = kopen(parent_path, 0);
// 	kfree(parent_path);
//
// 	if (!parent) {
// 		kfree(path);
// 		return 1;
// 	}
//
// 	int ret = 0;
// 	if (parent->symlink) {
// 		ret = parent->symlink(parent, target, f_path);
// 	} else {
// 		ret = 1;
// 	}
//
// 	kfree(path);
// 	close_fs(parent);
//
// 	return ret;
// }
//
// int unlink_fs(char *name) {
// 	fs_node_t *parent;
// 	char *cwd = (char *) PATH_SEPARATOR;
// 	char *path = canonicalize_path(cwd, name);
//
// 	char *parent_path = kmalloc(strlen(path) + 5);
// 	char * f_path = path + strlen(path) - 1;
// 	while (f_path > path) {
// 		if (*f_path == '/') {
// 			f_path += 1;
// 			break;
// 		}
// 		f_path--;
// 	}
//
// 	while (*f_path == '/') {
// 		f_path++;
// 	}
//
// 	parent = kopen(parent_path, 0);
// 	kfree(parent_path);
//
// 	if (!parent) {
// 		kfree(path);
// 		return 1;
// 	}
//
// 	int ret = 0;
// 	if (parent->unlink) {
// 		ret = parent->unlink(parent, f_path);
// 	} else {
// 		ret = 1;
// 	}
//
// 	kfree(path);
// 	close_fs(parent);
// 	return ret;
// }
//
// int readlink_fs(fs_node_t *node, char * buf, size_t size) {
// 	if (!node) return 1;
//
// 	if (node->readlink) {
// 		return node->readlink(node, buf, size);
// 	} else {
// 		return 1;
// 	}
// }
//
// fs_node_t *kopen_recur(const char *filename, uint32_t flags, uint32_t symlink_depth, char *relative_to) {
// 	if (!filename) {
// 		return NULL;
// 	}
//
// 	char *path = canonicalize_path(relative_to, filename);
// 	size_t path_len = strlen(path);
//
// 	if (path_len == 1) {
// 		fs_node_t *root_clone = kmalloc(sizeof(fs_node_t));
// 		memcpy(root_clone, fs_root, sizeof(fs_node_t));
//
// 		kfree(path);
//
// 		open_fs(root_clone, flags);
//
// 		return root_clone;
// 	}
//
// 	char *path_offset = path;
// 	uint64_t path_depth = 0;
// 	while (path_offset < path + path_len) {
// 		if (*path_offset == PATH_SEPARATOR) {
// 			*path_offset = '\0';
// 			path_depth++;
// 		}
// 		path_offset++;
// 	}
// 	path[path_len] = '\0';
// 	path_offset = path + 1;
//
// 	uint32_t depth = 0;
//
// 	fs_node_t *node_ptr = get_mount_point(path, path_depth, &path_offset, &depth);
//
// 	if (!node_ptr) return NULL;
//
// 	do {
// 		if ((node_ptr->flags & FS_SYMLINK) && depth == path_depth) {
// 			if (depth == path_depth - 1) {
// 				kfree((void *)path);
// 				kfree(node_ptr);
// 				return NULL;
// 			}
// 			if (symlink_depth >= MAX_SYMLINK_DEPTH) {
// 				kfree((void *)path);
// 				kfree(node_ptr);
// 				return NULL;
// 			}
// 			char symlink_buf[MAX_SYMLINK_SIZE];
// 			int len = readlink_fs(node_ptr, symlink_buf, sizeof(symlink_buf));
// 			if (len < 0) {
// 				kfree((void *)path);
// 				kfree(node_ptr);
// 				return NULL;
// 			}
// 			if (symlink_buf[len] != '\0') {
// 				kfree((void *)path);
// 				kfree(node_ptr);
// 				return NULL;
// 			}
// 			fs_node_t *old_node_ptr = node_ptr;
//
// 			char *relpath = kmalloc(path_len + 1);
// 			char *ptr = relpath;
// 			memcpy(relpath, path, path_len + 1);
// 			for (unsigned int i = 0; depth && i < depth-1; i++) {
// 				while(*ptr != '\0') {
// 					ptr++;
// 				}
// 				*ptr = PATH_SEPARATOR;
// 			}
// 			node_ptr = kopen_recur(symlink_buf, 0, symlink_depth + 1, relpath);
// 			kfree(relpath);
// 			kfree(old_node_ptr);
// 			if (!node_ptr) {
// 				kfree((void *)path);
// 				return NULL;
// 			}
// 		}
// 		if (path_offset >= path+path_len) {
// 			kfree(path);
// 			open_fs(node_ptr, flags);
// 			return node_ptr;
// 		}
// 		if (depth == path_depth) {
// 			open_fs(node_ptr, flags);
// 			kfree((void *)path);
// 			return node_ptr;
// 		}
// 		fs_node_t * node_next = finddir_fs(node_ptr, path_offset);
// 		kfree(node_ptr); /* Always a clone or an unopened thing */
// 		node_ptr = node_next;
// 		if (!node_ptr) {
// 			kfree((void *)path);
// 			return NULL;
// 		}
// 		path_offset += strlen(path_offset) + 1;
// 		++depth;
// 	} while (depth < path_depth + 1);
// 	kfree((void *)path);
// 	return NULL;
// }
//
// fs_node_t *kopen(const char *filename, uint32_t flags) {
//     kprintf("%s: %s\n", __func__, filename);
// 	return kopen_recur(filename, flags, 0, (char *) PATH_SEPARATOR);
// }
//
// void *vfs_mount(const char *path, fs_node_t *local_root) {
// 	if (!fs_tree) {
//         kprintf("%s: vfs structure not initalized\n", __func__);
// 		return NULL;
// 	}
// 	if (!path || path[0] != '/') {
//         kprintf("%s: absolute path required\n", __func__);
// 		return NULL;
// 	}
//
// 	tree_node_t *ret_val = NULL;
//
// 	char *p = strdup(path);
// 	char *i = p;
//
// 	int path_len = strlen(p);
//
// 	while (i < p + path_len) {
// 		if (*i == PATH_SEPARATOR) {
// 			*i = '\0';
// 		}
// 		i++;
// 	}
// 	p[path_len] = '\0';
// 	i = p + 1;
//
// 	tree_node_t *root_node = fs_tree->root;
//
// 	if (*i == '\0') {
// 		struct vfs_entry *root = (struct vfs_entry *) root_node->value;
// 		if (root->file) {
//             kprintf("%s: path %s already mounted\n", __func__, path);
// 		}
// 		root->file = local_root;
//
// 		fs_root = local_root;
// 		ret_val = root_node;
// 	} else {
// 		tree_node_t *node = root_node;
// 		char *at = i;
// 		while (1) {
// 			if (at >= p + path_len) {
// 				break;
// 			}
// 			int found = 0;
// 			foreach(child, node->children) {
// 				tree_node_t *tchild = (tree_node_t *) child->value;
// 				struct vfs_entry *entry = (struct vfs_entry *) tchild->value;
// 				if (!strcmp(entry->name, at)) {
// 					found = 1;
// 					node = tchild;
// 					ret_val = node;
// 					break;
// 				}
// 			}
//
// 			if (!found) {
// 				struct vfs_entry *entry = kmalloc(sizeof(struct vfs_entry));
// 				entry->name = strdup(at);
// 				entry->file = NULL;
// 				entry->device = NULL;
// 				entry->fs_type = NULL;
// 				node = tree_node_insert_child(fs_tree, node, entry);
// 			}
// 			at = at + strlen(at) + 1;
// 		}
//
// 		struct vfs_entry *entry = (struct vfs_entry *) node->value;
// 		if (entry->file) {
//             kprintf("%s: path %s already mounted\n", __func__, path);
// 		}
// 		entry->file = local_root;
// 		ret_val = node;
// 	}
//
// 	kfree(p);
// 	return ret_val;
// }
//
// fs_node_t *get_mount_point(char *path, uint32_t path_depth, char **outpath, uint32_t *outdepth) {
// 	size_t depth;
//
// 	for (depth = 0; depth <= path_depth; ++depth) {
// 		path += strlen(path) + 1;
// 	}
//
// 	fs_node_t   *last = fs_root;
// 	tree_node_t *node = fs_tree->root;
//
// 	char *at = *outpath;
// 	int _depth = 1;
// 	int _tree_depth = 0;
//
// 	while (1) {
// 		if (at >= path) {
// 			break;
// 		}
// 		int found = 0;
// 		foreach(child, node->children) {
// 			tree_node_t *tchild = (tree_node_t *) child->value;
// 			struct vfs_entry *entry = (struct vfs_entry *) tchild->value;
// 			if (!strcmp(entry->name, at)) {
// 				found = 1;
// 				node = tchild;
// 				at = at + strlen(at) + 1;
// 				if (entry->file) {
// 					_tree_depth = _depth;
// 					last = entry->file;
// 					*outpath = at;
// 				}
// 				break;
// 			}
// 		}
// 		if (!found) {
// 			break;
// 		}
// 		_depth++;
// 	}
//
// 	*outdepth = _tree_depth;
//
// 	if (last) {
// 		fs_node_t * last_clone = kmalloc(sizeof(fs_node_t));
// 		memcpy(last_clone, last, sizeof(fs_node_t));
// 		return last_clone;
// 	}
// 	return last;
// }
//
// int vfs_mount_type(const char *type, const char *arg, const char *mountpoint) {
// 	vfs_mount_callback t = (vfs_mount_callback)(uintptr_t) hashmap_get(fs_types, type);
// 	if (!t) {
// 		return 1;
// 	}
//
// 	fs_node_t *n = t(arg, mountpoint);
//
// 	if ((uintptr_t) n == (uintptr_t) 1) return 0;
//
// 	if (!n) return 1;
//
// 	tree_node_t *node = vfs_mount(mountpoint, n);
// 	if (node && node->value) {
// 		struct vfs_entry *entry = (struct vfs_entry *) node->value;
// 		entry->fs_type = strdup(type);
// 		entry->device  = strdup(arg);
// 	}
//
// 	return 0;
// }
//
// /* finally... */
// int vfs_register(const char *name, vfs_mount_callback callback) {
// 	if (hashmap_get(fs_types, name)) return 1;
// 	hashmap_set(fs_types, name, (void *)(uintptr_t) callback);
// 	return 0;
// }
//
// void map_vfs_directory(const char *c) {
// 	fs_node_t *f = vfs_mapper();
// 	struct vfs_entry *entry = vfs_mount((char*) c, f);
// 	if (!strcmp(c, "/")) {
// 		f->device = fs_tree->root;
// 	} else {
// 		f->device = entry;
// 	}
// }
//
// void init_vfs(void) {
// 	fs_tree = tree_create();
//
// 	struct vfs_entry* root = kmalloc(sizeof(struct vfs_entry));
//
// 	root->name = strdup("[root]");
// 	root->file = NULL; 
// 	root->fs_type = NULL;
// 	root->device = NULL;
//
// 	tree_set_root(fs_tree, root);
//
// 	fs_types = hashmap_create(5);
// }
