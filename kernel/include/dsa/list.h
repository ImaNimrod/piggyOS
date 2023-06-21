#pragma once

#include <memory/kheap.h>
#include <stdint.h>
#include <stddef.h>

typedef struct node {
	struct node *next, *prev;
	void* value;
	struct ListHeader* owner;
} __attribute__((packed)) node_t;

typedef struct ListHeader {
	node_t* head;
	node_t* tail;
	size_t length;
	const char* name;
	const void* metadata;
} __attribute__((packed)) list_t;

void list_destroy(list_t * list);
void list_free(list_t * list);
void list_append(list_t * list, node_t * item);
node_t * list_insert(list_t * list, void * item);
list_t * list_create(const char * name, const void * metadata);
node_t * list_find(list_t * list, void * value);
int list_index_of(list_t * list, void * value);
void list_remove(list_t * list, size_t index);
void list_delete(list_t * list, node_t * node);
node_t * list_pop(list_t * list);
node_t * list_dequeue(list_t * list);
list_t * list_copy(list_t * original);
node_t* list_insert_front(list_t* list, void* val);
void list_merge(list_t * target, list_t * source);
void * list_index(list_t * list, int index);
void * list_peek_front(list_t * list);
void list_append_after(list_t * list, node_t * before, node_t * node);
node_t * list_insert_after(list_t * list, node_t * before, void * item);
void list_append_before(list_t * list, node_t * after, node_t * node);
node_t * list_insert_before(list_t * list, node_t * after, void * item);

#ifndef LIST_NO_FOREACH
#  define foreach(i, list) for (node_t * i = (list)->head; i != NULL; i = i->next)
#  define foreachr(i, list) for (node_t * i = (list)->tail; i != NULL; i = i->prev)
#endif

