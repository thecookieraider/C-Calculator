#ifndef HASHMAP_H
#define HASHMAP_H

#include "util.h"
#include <string.h>
#include <stdint.h>

#define INITIAL_SIZE (256)

struct hashmap_element
{
	char * key;
	void * data;
	hashmap_element * next;
	hashmap_element * prev;
};

struct hashmap
{
	int table_size;
	int size;
	struct hashmap_element * elements;
};

uint32_t hash(char * key, int hashSize)
{
	uint32_t hashVal;
	for (hashVal = 0; *key != '\0'; key++) {
		hashVal = *key + 31 * hashVal;
	}

	return hashVal % hashSize;
}

hashmap * new_hashmap()
{
	hashmap * map = (struct hashmap *)MallocOrDie(sizeof(struct hashmap));
	map->elements = (struct hashmap_element *)calloc(INITIAL_SIZE, sizeof(struct hashmap_element));

	map->table_size = INITIAL_SIZE;
	map->size = 0;

	return map;
}

struct hashmap_element * hashmap_get(struct hashmap * map, char * s)
{
	struct hashmap_element * itr;
	for (itr = &map->elements[hash(s, map->table_size)]; itr != NULL; itr = itr->next) {
		if (strcmp(s, itr->key) == 0) {
			return itr;
		}
	}

	return NULL;
}

struct hashmap_element * hashmap_put(struct hashmap * map, char * key, void * value)
{
	struct hashmap_element * e;
	uint32_t hashVal;

	if ((e = hashmap_get(map, key)) == NULL) {
		e = (struct hashmap_element *)MallocOrDie(sizeof(struct hashmap_element));
		e->data = value;
		e->key = key;
		hashVal = hash(key, map->table_size);
		e->next = &map->elements[hashVal];
		map->elements[hashVal].prev = e;
		map->elements[hashVal] = *e;
		map->size++;
	} else {
		free(e->data);
		e->data = value;
	}

	return e;
}

void hashmap_remove(struct hashmap * map, char * key)
{
	struct hashmap_element * e;
	uint32_t hashVal;

	if ((e = hashmap_get(map, key)) == NULL) return;
	else {
		map->elements[hash(e->next->key, map->table_size)] = *e->next;
		e->prev->next = NULL;
		free(e);
	} 
}
#endif