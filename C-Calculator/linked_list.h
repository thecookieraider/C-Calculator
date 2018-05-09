#ifndef LINKED_LIST_H
#define LINKED_LIST_H
/* Linked List */

/* 
	This file just defines a very basic singly linked list. 
	It is up to you to determine the data types and implement your own find / sort functions based on this chosen data type.
	
*/
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

#define LIST_INIT(list) list = (struct SinglyLinkedList *)malloc(sizeof(struct SinglyLinkedList)); \
						list->head = NULL; \
						list->tail = NULL;

/* Use this instead of malloc everywhere. */

struct Node
{
	void * data;
	struct Node * next;
	struct Node * prev;
};

struct SinglyLinkedList
{
	struct Node * head;
	struct Node * tail;
};

void linkedlist_push_front(struct SinglyLinkedList * list, void * new_data, size_t data_size)
{
	if (!list->head) {
		list->head = (struct Node *)MallocOrDie(sizeof(struct Node));
		list->head->data = new_data;
		list->head->next = NULL;
		list->head->prev = NULL;
	} else {
		struct Node * new_node = (struct Node *)MallocOrDie(sizeof(struct Node));
		new_node->data = new_data;
		new_node->next = list->head;
		new_node->prev = NULL;
		list->head = new_node;
	}
}

void linkedlist_push_back(struct SinglyLinkedList * list, void * new_data)
{
	if (!list->head) {
		list->head = (struct Node *)MallocOrDie(sizeof(struct Node));
		list->head->data = new_data;
		list->head->next = NULL;
		list->head->prev = NULL;
		list->tail = list->head;
	} else if(!list->tail) {
		struct Node * itr = list->head;

		while (itr->next) {
			itr = list->head->next;
		}

		itr->next = (struct Node *)MallocOrDie(sizeof(struct Node));
		itr->next->data = new_data;
		itr->next->next = NULL;
		itr->next->prev = itr;
		list->tail = itr->next;
	} else {
		list->tail->next = (struct Node *)MallocOrDie(sizeof(struct Node));
		list->tail->next->data = new_data;
		list->tail->next->next = NULL;
		list->tail->next->prev = list->tail;
		list->tail = list->tail->next;
	}
}

void linkedlist_pop(struct SinglyLinkedList * list)
{
	struct Node * to_destroy = list->tail;
	list->tail = list->tail->prev;
	free(to_destroy);
}

size_t linkedlist_sizeOfList(struct SinglyLinkedList * list)
{
	size_t count = 0;
	struct Node * itr = list->head;
	while (itr) {
		count++;
		itr = itr->next;
	}

	return count;
}

void linkedlist_free(struct SinglyLinkedList * list)
{
	struct Node * n = list->head;
	struct Node * toFree = n;
	while (n) {
		n = n->next;
		free(toFree);
		toFree = n;
	}
}

#endif