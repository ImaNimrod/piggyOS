#ifndef _KERNEL_LIST_H
#define _KERNEL_LIST_H

#include <memory/kheap.h>
#include <stddef.h>
#include <stdint.h>

typedef struct listnode {
	struct listnode *prev, *next;
	void* value;
} list_node_t;

typedef struct list{
	list_node_t *head, *tail;
	size_t size;
} list_t;

#define foreach(t, list) for(list_node_t* t = list->head; t != NULL; t = t->next)

/* function declarations */
list_t* list_create(void);
size_t list_size(list_t* list);
list_node_t* list_insert_front(list_t* list, void* value);
list_node_t* list_insert_back(list_t* list, void* value);
void* list_remove_node(list_t* list, list_node_t* node);
void* list_remove_front(list_t* list);
void* list_remove_back(list_t* list);
void* list_peek_front(list_t* list);
void* list_peek_back(list_t* list);
list_node_t* list_pop(list_t* list);
void list_destroy(list_t* list);
void listnode_destroy(list_node_t* node);
list_node_t* list_contain(list_t* list, void* value);
list_node_t* list_get_node_by_index(list_t* list, size_t index);
void* list_remove_by_index(list_t* list, size_t index);
void list_merge(list_t* target, list_t* source);

#endif
