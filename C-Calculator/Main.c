#include "linked_list.h"
#include <stdio.h>

int main()
{
	/* Test for singly linked list */
	struct SinglyLinkedList * list;
	LIST_INIT(list)

	char * test = "This is a test";
	char * test2 = "This is also a test";

	linkedlist_push_back(list, (void *)test);
	linkedlist_push_back(list, (void *)test2);
	printf("%s - %d", (char *)list->tail->data, linkedlist_sizeOfList(list));
	getchar();
	linkedlist_pop(list);
	printf("%s", (char*)list->tail->data);
	getchar();
}