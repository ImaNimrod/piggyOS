#include <dsa/list.h>

list_t* list_create() {
	list_t* list = kcalloc(sizeof(list_t), 1);
	return list;
}

size_t list_size(list_t* list) {
    if(!list) return 0;
	return list->size;
}

void* list_remove_node(list_t* list, list_node_t* node) {
    void* value = node->value;
    if(list->head == node)
        return list_remove_front(list);
    else if(list->tail == node)
        return list_remove_back(list);
    else {
        node->next->prev = node->prev;
        node->prev->next = node->next;
        list->size--;
        kfree(node);
    }
    return value;
}

list_node_t* list_insert_front(list_t* list, void* value) {
	list_node_t* t = kcalloc(sizeof(list_node_t), 1);
	list->head->prev = t;
    t->next = list->head;
	t->value = value;

	if(!list->head)
		list->tail = t;

	list->head = t;
	list->size++;
	return t;
}

void list_insert_back(list_t* list, void* value) {
	list_node_t* t = kcalloc(sizeof(list_node_t), 1);
	t->prev = list->tail;
    if(list->tail) {
        list->tail->next = t;
    }
	t->value = value;

	if(!list->head) {
		list->head = t;
    }

	list->tail = t;
	list->size++;
}

void* list_remove_front(list_t* list) {
	if(!list->head) return NULL;
	list_node_t* t = list->head;
    void* value = t->value;
	list->head = t->next;
	if(list->head)
		list->head->prev = NULL;
	kfree(t);
	list->size--;
    return value;
}

void* list_remove_back(list_t* list) {
	if(!list->head) return NULL;
	list_node_t* t = list->tail;
    void* value = t ->value;
	list->tail = t->prev;
	if(list->tail)
		list->tail->next = NULL;
	kfree(t);
	list->size--;
    return value;
}

void* list_peek_front(list_t* list) {
	if(!list->head) return NULL;
	return list->head->value;
}

void* list_peek_back(list_t* list) {
	if(!list->tail) return NULL;
	return list->tail->value;
}

list_node_t* list_pop(list_t* list) {
	if(!list->head) return NULL;
	list_node_t* t = list->tail;
	list->tail = t->prev;

	if(list->tail)
		list->tail->next = NULL;

	list->size--;
	return t;
}

list_node_t* list_contain(list_t* list, void* value) {
    size_t idx = 0;
    foreach(listnode, list) {
        if(listnode->value == value)
            return listnode;
        idx++;
    }
    return NULL;
}

list_node_t* list_get_node_by_index(list_t* list, size_t index) {
    if(index >= list_size(list))
        return NULL;
    uint32_t curr = 0;
    foreach(listnode, list) {
        if(index == curr) return listnode;
        curr++;
    }
    return NULL;
}

void* list_remove_by_index(list_t* list, size_t index) {
    list_node_t* node = list_get_node_by_index(list, index);
    return list_remove_node(list, node);
}

void list_destroy(list_t* list) {
	list_node_t* node = list->head;
	while(node != NULL) {
		list_node_t* save = node;
		node = node->next;
		kfree(save);
	}
	kfree(list);
}

void listnode_destroy(list_node_t * node) {
	kfree(node);
}

void list_merge(list_t* target, list_t* source) {
	if (source->head)
		source->head->prev = target->tail;

	if (target->tail) {
		target->tail->next = source->head;
	} else {
		target->head = source->head;
	}

	if (source->tail)
		target->tail = source->tail;

	target->size += source->size;
	kfree(source);
}
