/*
Copied from
https://github.com/SanderMertens/flecs/blob/master/src/datastructures/map.c
*/

#pragma once
#include <stdint.h>


typedef struct map_entry_t
{
	uint64_t key;
	uint64_t value;
	struct map_entry_t *next;
} map_entry_t;

typedef struct
{
	map_entry_t *first;
} map_bucket_t;

typedef struct
{
	uint8_t bucket_shift;
	int32_t count;
	int32_t bucket_count;
	map_bucket_t *buckets;
} map_t;

typedef struct map_iter_t {
    const map_t *map;
    map_bucket_t *bucket;
    map_entry_t *entry;
    uint64_t *res;
} map_iter_t;


void map_init(map_t *map);

void map_fini(map_t *map);

void map_insert(map_t *map, uint64_t key, uint64_t value);

void* map_insert_alloc(map_t *map, int elem_size, uint64_t key);

uint64_t *map_ensure(map_t *map, uint64_t key);

void *map_ensure_alloc(map_t *map, int elem_size, uint64_t key);

uint64_t *map_get(map_t const *map, uint64_t key);

void* map_get_deref(const map_t *map, uint64_t key);

uint64_t map_remove(map_t *map, uint64_t key);

map_iter_t map_iter(const map_t *map);

int map_next(map_iter_t *iter);

#define map_key(it) ((it)->res[0])
#define map_value(it) ((it)->res[1])