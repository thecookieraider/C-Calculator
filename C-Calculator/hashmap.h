#ifndef HASHMAP_H
#define HASHMAP_H

#include "util.h"
#include <string.h>
#include <stdint.h>
#include <math.h>

//The initial size of any hashmap. Adjust if needed.
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

//The hash function is just the java.lang.String hash function.
//Details can be found here: https://en.wikipedia.org/wiki/Java_hashCode()#The_java.lang.String_hash_function
uint32_t hash(char * key, int32_t hashSize)
{
	double hashVal;
	size_t keyLength = strlen(key);
	for (size_t i = 0; i < keyLength; i++) {
		hashVal = key[i] * pow(31, keyLength - 1 - i);
	}

	return (int32_t)hashVal % hashSize;
}

//Rehashes a given hashmap into a address space that is 
//twice the map's current address space size. The old space
//is freed before the function returns
void hashmap_rehash(struct hashmap * map)
{
	//Save the current hash space since we are going to need to rehash it all
	struct hashmap_element * curr = map->elements;
	//Setup the new hash space (should be double the current hash space in size)
	struct hashmap_element * temp = (struct hashmap_element *)calloc(2 * map->table_size, sizeof(struct hashmap_element));
	//Save the old size so we can iterate over the old hash space
	uint32_t old_size = map->table_size;
	//Update the hashmap fields
	map->table_size *= 2;
	map->elements = temp;
	//Go through the old space and hash each item into the new space. O(n)
	for (uint32_t i = 0; i < old_size; i++){
		if (curr[i].key != NULL) {
			hashmap_put(map, curr[i].key, curr[i].data);
		}
	}

	free(curr);
}

//Sets up a new hashmap. The initial size of this table
//will be set to the macro INITIAL_SIZE. Redefine hte macro if you 
//want the initial size to be greater or less
struct hashmap * new_hashmap()
{
	struct hashmap * map = (struct hashmap *)MallocOrDie(sizeof(struct hashmap));
	map->elements = (struct hashmap_element *)calloc(INITIAL_SIZE, sizeof(struct hashmap_element));

	map->table_size = INITIAL_SIZE;
	map->size = 0;

	return map;
}

//Gets a value from a hashmap given a key. Will return NULL if the key is not found.
struct hashmap_element * hashmap_get(struct hashmap * map, char * s)
{
	uint32_t hashValue = hash(s, map->table_size);
	//Since the table function defined in this header file makes use of linear probing,
	//we are going to need to seach nearby pairs to see if the given key was moved to some other cell
	//due to a collision
	uint32_t stopHere = hashValue;

	if (map->elements[hashValue].key == NULL) return NULL;
	else {
		if (strcmp(s, map->elements[hashValue].key) == 0) {
			return &map->elements[hashValue];
		}
	}

	for (hashValue = (hashValue + 1) % map->table_size; hashValue != stopHere && map->elements[hashValue].key != NULL; hashValue = (hashValue + 1) % map->table_size) {
		if (strcmp(s, map->elements[hashValue].key) == 0) {
			return &map->elements[hashValue];
		}
	}

	return NULL;
}

//Put a key-value pair into a given hashmap. This function makes use of linear probing
//to handle any collisions brought about by the hash function in this same header file.
//A pointer to the freshly added element is returned
struct hashmap_element * hashmap_put(struct hashmap * map, char * key, void * value)
{
	struct hashmap_element * e;
	uint32_t hashVal = hash(key, map->table_size);

	if ((e = hashmap_get(map, key)) == NULL) {
		hashVal = hash(key, map->table_size);
		map->elements[hashVal].data = value;
		map->elements[hashVal].key = key;

		map->size++;

		e = &map->elements[hashVal];
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

//Removes a key-value pair from a given hashmap.
//This function will also try to find a key-value pair that has an earlier
//or equal hash to the removed key-value pair to move up to the removed
//key-value pair's empty position. It will repeat this process for the newly vacant spot
//of the moved node. If a NULL pair is found at any point, this process terminates.
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

//Frees up any data in use by a hashmap. Sets the map pointer to NULL
void hashmap_free(struct hashmap * map)
{
	free(map->elements);
	free(map);
	map = NULL;
}

typedef float(*Function)(float, float);

//Forward define this function since it is used in a good amount of other func_hashmap functions
struct func_hashmap_element * func_hashmap_put(struct func_hashmap * map, char * key, void * value);

struct func_hashmap_element
{
	char * key;
	Function data;
};

struct func_hashmap
{
	uint32_t table_size;
	uint32_t size;
	struct func_hashmap_element * elements;
};

//Rehashes a given func_hashmap into a address space that is 
//twice the map's current address space size. The old space
//is freed before the function returns
void func_hashmap_rehash(struct func_hashmap * map)
{
	//Save the current hash space since we are going to need to rehash it all
	struct func_hashmap_element * curr = map->elements;
	//Setup the new hash space (should be double the current hash space in size)
	struct func_hashmap_element * temp = (struct func_hashmap_element *)calloc(2 * map->table_size, sizeof(struct func_hashmap_element));
	//Save the old size so we can iterate over the old hash space
	uint32_t old_size = map->table_size;
	//Update the func_hashmap fields
	map->table_size *= 2;
	map->elements = temp;
	//Go through the old space and hash each item into the new space. O(n)
	for (uint32_t i = 0; i < old_size; i++) {
		if (curr[i].key != NULL) {
			func_hashmap_put(map, curr[i].key, curr[i].data);
		}
	}

	free(curr);
}

//Sets up a new func_hashmap. The initial size of this table
//will be set to the macro INITIAL_SIZE. Redefine hte macro if you 
//want the initial size to be greater or less
struct func_hashmap * new_func_hashmap()
{
	struct func_hashmap * map = (struct func_hashmap *)MallocOrDie(sizeof(struct func_hashmap));
	map->elements = (struct func_hashmap_element *)calloc(INITIAL_SIZE, sizeof(struct func_hashmap_element));

	map->table_size = INITIAL_SIZE;
	map->size = 0;

	return map;
}

//Gets a value from a func_hashmap given a key. Will return NULL if the key is not found.
struct func_hashmap_element * func_hashmap_get(struct func_hashmap * map, char * s)
{
	uint32_t hashValue = hash(s, map->table_size);
	//Since the table function defined in this header file makes use of linear probing,
	//we are going to need to seach nearby pairs to see if the given key was moved to some other cell
	//due to a collision
	uint32_t stopHere = hashValue;

	if (map->elements[hashValue].key == NULL) return NULL;
	else {
		if (strcmp(s, map->elements[hashValue].key) == 0) {
			return &map->elements[hashValue];
		}
	}

	for (hashValue = (hashValue + 1) % map->table_size; hashValue != stopHere && map->elements[hashValue].key != NULL; hashValue = (hashValue + 1) % map->table_size) {
		if (strcmp(s, map->elements[hashValue].key) == 0) {
			return &map->elements[hashValue];
		}
	}

	return NULL;
}

//Put a key-value pair into a given func_hashmap. This function makes use of linear probing
//to handle any collisions brought about by the hash function in this same header file.
//A pointer to the freshly added element is returned
struct func_hashmap_element * func_hashmap_put(struct func_hashmap * map, char * key, Function value)
{
	struct func_hashmap_element * e;
	uint32_t hashVal = hash(key, map->table_size);

	if ((e = func_hashmap_get(map, key)) == NULL) {
		hashVal = hash(key, map->table_size);
		map->elements[hashVal].data = value;
		map->elements[hashVal].key = key;

		map->size++;

		e = &map->elements[hashVal];
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

//Removes a key-value pair from a given func_hashmap.
//This function will also try to find a key-value pair that has an earlier
//or equal hash to the removed key-value pair to move up to the removed
//key-value pair's empty position. It will repeat this process for the newly vacant spot
//of the moved node. If a NULL pair is found at any point, this process terminates.
void func_hashmap_remove(struct func_hashmap * map, char * key)
{
	struct func_hashmap_element * e;
	uint32_t hashVal = hash(key, map->table_size);
	uint32_t i;
	if ((e = func_hashmap_get(map, key)) == NULL) return;
	else {
		e->key = NULL;
		e->data = NULL;
		for (i = hashVal + 1; i != map->table_size; i = (i + 1) % map->table_size) {
			if (map->elements[i].key == NULL) {
				break;
			} else if (hash(map->elements[i].key, map->size <= hashVal)) {
				*e = map->elements[i];
				e = &map->elements[i];
				hashVal = i;
			}
		}
	}
}

//Frees up any data in use by a func_hashmap. Sets the map pointer to NULL
void func_hashmap_free(struct func_hashmap * map)
{
	free(map->elements);
	free(map);
	map = NULL;
}

#endif