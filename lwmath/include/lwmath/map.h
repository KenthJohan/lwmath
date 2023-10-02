/*
Copied from
https://github.com/SanderMertens/flecs/blob/master/src/datastructures/map.c
*/

#pragma once
#include <stdint.h>
#include <stddef.h>

#define map_container_of(ptr, type, member) ((type *) ((char *)(ptr) - offsetof(type, member)))
#define map_entry(ptr, type, member) map_container_of(ptr, type, member)

typedef struct map_entry_t
{
	struct map_entry_t *next;
	uint64_t key;
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

typedef struct map_iter_t
{
	map_bucket_t *bucket;
	map_bucket_t *end;
	map_entry_t *entry;
} map_iter_t;

void map_init(map_t *map);

void map_fini(map_t *map);

void map_insert(map_t *map, uint64_t key, map_entry_t * entry);

map_entry_t *map_get(map_t const *map, uint64_t key);

map_entry_t *map_remove(map_t *map, uint64_t key);

#define map_remove_t(map, key, t, entry) map_entry(map_remove(map, key), t, entry)



map_iter_t map_iter(const map_t *map);

map_entry_t * map_next(map_iter_t *iter);

