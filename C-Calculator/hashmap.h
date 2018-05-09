#ifndef HASHMAP_H
#define HASHMAP_H

#include "util.h"
#include <string.h>
#include <stdint.h>

//The initial size of any hashmap. Adjust if needed. The rehash function will allow you to double your
//address space if need be
#define INITIAL_SIZE (256)

//Forward define this function since it is used in a good amount of other hashmap functions
struct hashmap_element * hashmap_put(struct hashmap * map, char * key, void * value);

struct hashmap_element
{
	char * key;
	void * data;
};

struct hashmap
{
	uint32_t table_size;
	uint32_t size;
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

void hashmap_rehash(struct hashmap * map)
{
	struct hashmap_element * curr = map->elements;
	struct hashmap_element * temp = (struct hashmap_element *)calloc(2 * map->table_size, sizeof(struct hashmap_element));

	uint32_t old_size = map->table_size;

	map->table_size *= 2;
	map->elements = temp;

	for (uint32_t i = 0; i < old_size; i++){
		if (curr[i].key != NULL) {
			hashmap_put(map, curr[i].key, curr[i].data);
		}
	}

	free(curr);
}

struct hashmap * new_hashmap()
{
	struct hashmap * map = (struct hashmap *)MallocOrDie(sizeof(struct hashmap));
	map->elements = (struct hashmap_element *)calloc(INITIAL_SIZE, sizeof(struct hashmap_element));

	map->table_size = INITIAL_SIZE;
	map->size = 0;

	return map;
}

struct hashmap_element * hashmap_get(struct hashmap * map, char * s)
{
	uint32_t hashValue = hash(s, map->table_size);

	uint32_t stopHere = hashValue;

	for (hashValue; hashValue != stopHere || map->elements[hashValue].key != NULL; hashValue = (hashValue + 1) % map->table_size) {
		if (strcmp(s, map->elements[hashValue].key) == 0) {
			return &map->elements[hashValue];
		}
	}

	return NULL;
}

struct hashmap_element * hashmap_put(struct hashmap * map, char * key, void * value)
{
	struct hashmap_element * e;
	uint32_t hashVal = hash(key, map->table_size);

	if ((e = hashmap_get(map, key)) == NULL) {
		hashVal = hash(key, map->table_size);
		map->elements[hashVal].data = value;
		map->elements[hashVal].key = key;

		map->size++;
	} else {
		if (map->elements[hashVal].key != key) {
			hashVal = (hashVal + 1) % map->table_size;
			while (map->elements[hashVal].key != NULL) { hashVal = (hashVal + 1) % map->table_size; }
			map->elements[hashVal].data = value;
			map->elements[hashVal].key = key;
		} else {
			e->data = value;
		}
	}

	return e;
}

void hashmap_remove(struct hashmap * map, char * key)
{
	struct hashmap_element * e;
	uint32_t hashVal = hash(key, map->table_size);
	uint32_t i;
	if ((e = hashmap_get(map, key)) == NULL) return;
	else {
		e->key = NULL;
		e->data = NULL;
		for (i = hashVal+1; i != map->table_size; i = (i + 1) % map->table_size) {
			if (map->elements[i].key == NULL) {
				break;
			} else if(hash(map->elements[i].key, map->size <= hashVal)){
				*e = map->elements[i];
				e = &map->elements[i];
				hashVal = i;
			}
		}
	} 
}

void hashmap_free(struct hashmap * map)
{
	free(map->elements);
}

#endif