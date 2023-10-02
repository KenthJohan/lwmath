/*
Copied some code from
https://github.com/SanderMertens/flecs/blob/master/src/datastructures/map.c


*/

#include "lwmath/map.h"
#include "lwmath_assert.h"
#include <stdlib.h>
#include <string.h>

#define MAP_LOAD_FACTOR (12)

static uint8_t map_log2(uint32_t v)
{
	static const uint8_t log2table[32] =
		{0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
		 8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31};
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return log2table[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}

static uint8_t get_shift(int32_t bucket_count)
{
	return (uint8_t)(64u - map_log2((uint32_t)bucket_count));
}

static int32_t next_pow_of_2(int32_t n)
{
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;

	return n;
}

static int32_t get_index(uint16_t bucket_shift, uint64_t key)
{
	return (int32_t)((11400714819323198485ull * key) >> bucket_shift);
}

static int32_t map_get_bucket_count(int32_t count)
{
	return next_pow_of_2((int32_t)(count * MAP_LOAD_FACTOR * 0.1));
}

static void map_rehash(map_t *map, int32_t count)
{
	count = next_pow_of_2(count);
	if (count < 2)
	{
		count = 2;
	}
	lwmath_assert(count > map->bucket_count, "");

	int32_t old_count = map->bucket_count;
	map_bucket_t *buckets = map->buckets;
	map_bucket_t *b;
	map_bucket_t *end = buckets + old_count;

	map->buckets = calloc(1, sizeof(map_bucket_t) * count);
	map->bucket_count = count;
	map->bucket_shift = get_shift(count);

	/* Remap old bucket entries to new buckets */
	for (b = buckets; b < end; b++)
	{
		map_entry_t *entry;
		for (entry = b->first; entry;)
		{
			map_entry_t *next = entry->next;
			int32_t bucket_index = get_index(map->bucket_shift, entry->key);
			map_bucket_t *bucket = &map->buckets[bucket_index];
			entry->next = bucket->first;
			bucket->first = entry;
			entry = next;
		}
	}

	free(buckets);
}

static map_bucket_t *map_get_bucket(const map_t *map, uint64_t key)
{
	lwmath_assert(map != NULL, "");
	int32_t bucket_id = get_index(map->bucket_shift, key);
	lwmath_assert(bucket_id < map->bucket_count, "");
	return &map->buckets[bucket_id];
}

static map_entry_t *bucket_get_entry(map_bucket_t *bucket, uint64_t key)
{
	map_entry_t *entry;
	for (entry = bucket->first; entry; entry = entry->next)
	{
		if (entry->key == key)
		{
			return entry;
		}
	}
	return NULL;
}

static void bucket_add(map_bucket_t *bucket, uint64_t key, map_entry_t *new_entry)
{
	new_entry->key = key;
	new_entry->next = bucket->first;
	bucket->first = new_entry;
}

static map_entry_t *map_bucket_remove(map_t *map, map_bucket_t *bucket, uint64_t key)
{
	map_entry_t *entry;
	for (entry = bucket->first; entry; entry = entry->next)
	{
		if (entry->key == key)
		{
			map_entry_t **next_holder = &bucket->first;
			while (*next_holder != entry)
			{
				next_holder = &(*next_holder)->next;
			}
			*next_holder = entry->next;
			map->count--;
			return entry;
		}
	}
	return entry;
}

void map_init(map_t *map, int32_t n)
{
	memset(map, 0, sizeof(map_t));
	map_rehash(map, n);
}

void map_fini(map_t *map)
{
	if (map->bucket_shift == 0)
	{
		return;
	}
	free(map->buckets);
	memset(map, 0, sizeof(map_t));
}

map_entry_t *map_get(map_t const *map, uint64_t key)
{
	map_bucket_t *bucket = map_get_bucket(map, key);
	return bucket_get_entry(bucket, key);
}

void map_insert(map_t *map, uint64_t key, map_entry_t *entry)
{
	lwmath_assert(map_get(map, key) == NULL, "Key already set");
	int32_t map_count = ++map->count;
	int32_t tgt_bucket_count = map_get_bucket_count(map_count);
	int32_t bucket_count = map->bucket_count;
	if (tgt_bucket_count > bucket_count)
	{
		map_rehash(map, tgt_bucket_count);
	}
	map_bucket_t *bucket = map_get_bucket(map, key);
	bucket_add(bucket, key, entry);
}

map_entry_t *map_remove(map_t *map, uint64_t key)
{
	map_bucket_t *bucket = map_get_bucket(map, key);
	return map_bucket_remove(map, bucket, key);
}

map_iter_t map_iter(const map_t *map)
{
	if (map->bucket_shift == 0)
	{
		return (map_iter_t){0};
	}
	map_iter_t it = {
		.entry = map->buckets->first,
		.bucket = map->buckets,
		.end = map->buckets + map->bucket_count};
	return it;
}

map_entry_t *map_next(map_iter_t *iter)
{
	map_entry_t *entry = iter->entry;

	// Check if current bucket has entries, if so then return that entry:
	if (entry != NULL)
	{
		iter->entry = entry->next;
		return entry;
	}

	// At this point current bucket does not have any entries 
	// so we need to find a bucket that contains entries.
	// Lets find first non empty bucket and return first entry:
	do
	{
		++iter->bucket;
		if (iter->bucket == iter->end)
		{
			return NULL;
		}
		entry = iter->bucket->first;
	} while (entry == NULL);
	
	lwmath_assert(entry != NULL, "");

	iter->entry = entry->next;

	return entry;
}


void map_clear(map_t *map)
{
	map->count = 0;
	memset(map->buckets, 0, sizeof(map_bucket_t) * map->bucket_count);
}