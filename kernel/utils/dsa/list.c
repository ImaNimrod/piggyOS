#include <string.h>
#include <dsa/list.h>

void list_destroy(list_t* list) {
	node_t* n = list->head;
	while (n) {
		kfree(n->value);
		n = n->next;
	}
}

void list_free(list_t* list) {
	node_t* n = list->head;
	while (n) {
		node_t* s = n->next;
		kfree(n);
		n = s;
	}
}

void* list_peek_front(list_t * list) {
	if(!list->head) return NULL;
	return list->head->value;
}

void list_append(list_t* list, node_t* node) {
	node->next = NULL;
	node->owner = list;

	if (!list->length) {
		list->head = node;
		list->tail = node;
		node->prev = NULL;
		node->next = NULL;
		list->length++;
		return;
	}

	list->tail->next = node;
	node->prev = list->tail;
	list->tail = node;
	list->length++;
}

node_t* list_insert(list_t* list, void* item) {
	node_t* node = kmalloc(sizeof(node_t));
	node->value = item;
	node->next  = NULL;
	node->prev  = NULL;
	node->owner = NULL;
	list_append(list, node);

	return node;
}

void list_append_after(list_t* list, node_t* before, node_t* node) {
	node->owner = list;
	if (!list->length) {
		list_append(list, node);
		return;
	}
	if (before == NULL) {
		node->next = list->head;
		node->prev = NULL;
		list->head->prev = node;
		list->head = node;
		list->length++;
		return;
	}
	if (before == list->tail) {
		list->tail = node;
	} else {
		before->next->prev = node;
		node->next = before->next;
	}
	node->prev = before;
	before->next = node;
	list->length++;
}

node_t* list_insert_after(list_t* list, node_t* before, void* item) {
	node_t* node = kmalloc(sizeof(node_t));
	node->value = item;
	node->next  = NULL;
	node->prev  = NULL;
	node->owner = NULL;
	list_append_after(list, before, node);
	return node;
}

void list_append_before(list_t* list, node_t* after, node_t* node) {
    node->owner = list;
	if (!list->length) {
		list_append(list, node);
		return;
	}

	if (after == NULL) {
		node->next = NULL;
		node->prev = list->tail;
		list->tail->next = node;
		list->tail = node;
		list->length++;
		return;
	}

	if (after == list->head) {
		list->head = node;
	} else {
		after->prev->next = node;
		node->prev = after->prev;
	}
	node->next = after;
	after->prev = node;
	list->length++;
}

node_t* list_insert_before(list_t* list, node_t* after, void* item) {
	node_t* node = kmalloc(sizeof(node_t));
	node->value = item;
	node->next  = NULL;
	node->prev  = NULL;
	node->owner = NULL;
	list_append_before(list, after, node);
	return node;
}

list_t* list_create(const char* name, const void* metadata) {
	list_t* out = kmalloc(sizeof(list_t));
	out->head = NULL;
	out->tail = NULL;
	out->length = 0;
	out->name = name;
	out->metadata = metadata;
	return out;
}

node_t* list_find(list_t* list, void* value) {
	foreach(item, list) {
		if (item->value == value) {
			return item;
		}
	}
	return NULL;
}

int list_index_of(list_t* list, void* value) {
	int i = 0;
	foreach(item, list) {
		if (item->value == value) {
			return i;
		}
		i++;
	}
	return 1;
}

void* list_index(list_t* list, int index) {
	int i = 0;

	foreach(item, list) {
		if (i == index) return item->value;
		i++;
	}
	return NULL;
}

void list_remove(list_t* list, size_t index) {
	if (index > list->length) return;
	size_t i = 0;
	node_t * n = list->head;
	while (i < index) {
		n = n->next;
		i++;
	}
	list_delete(list, n);
}

node_t* list_insert_front(list_t* list, void* val) {
	node_t* t = kcalloc(sizeof(node_t), 1);
	list->head->prev = t;
    t->next = list->head;
	t->value = val;

	if(!list->head)
		list->tail = t;

	list->head = t;
	list->length++;
	return t;
}

void list_delete(list_t* list, node_t* node) {
	if (node == list->head) {
		list->head = node->next;
	}
	if (node == list->tail) {
		list->tail = node->prev;
	}
	if (node->prev) {
		node->prev->next = node->next;
	}
	if (node->next) {
		node->next->prev = node->prev;
	}

	node->prev = NULL;
	node->next = NULL;
	node->owner = NULL;
	list->length--;
    kfree(node);
}


node_t* list_pop(list_t* list) {
	if (!list->tail) return NULL;
	node_t* out = list->tail;
	list_delete(list, out);
	return out;
}

node_t* list_dequeue(list_t* list) {
	if (!list->head) return NULL;
	node_t* out = list->head;
	list_delete(list, out);
	return out;
}

list_t* list_copy(list_t* original) {
	list_t* out = list_create(original->name, original->metadata);
	node_t* node = original->head;
	while (node) {
		list_insert(out, node->value);
	}
	return out;
}

void list_merge(list_t* target, list_t* source) {
	foreach(node, source) {
		node->owner = target;
	}
	if (source->head) {
		source->head->prev = target->tail;
	}
	if (target->tail) {
		target->tail->next = source->head;
	} else {
		target->head = source->head;
	}
	if (source->tail) {
		target->tail = source->tail;
	}
	target->length += source->length;
	kfree(source);
}
