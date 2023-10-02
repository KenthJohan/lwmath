#include "lwmath/map.h"
#include "lwmath_assert.h"
#include <stdlib.h>
#include <string.h>

#define MAP_LOAD_FACTOR (12)
#define MAP_OFFSET(o, offset) (void *)(((uintptr_t)(o)) + ((uintptr_t)(offset)))
#define MAP_ELEM(ptr, size, index) MAP_OFFSET(ptr, (size) * (index))
#define MAP_ELEM_T(o, T, index) MAP_ELEM(o, sizeof(T), index)
#define MAP_BUCKET_END(b, c) MAP_ELEM_T(b, map_bucket_t, c)

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
	map_bucket_t *end = MAP_BUCKET_END(buckets, old_count);

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

static uint64_t *bucket_get_value(map_bucket_t *bucket, uint64_t key)
{
	map_entry_t *entry;
	for (entry = bucket->first; entry; entry = entry->next)
	{
		if (entry->key == key)
		{
			return &entry->value;
		}
	}
	return NULL;
}

static uint64_t *bucket_add(map_bucket_t *bucket, uint64_t key)
{
	map_entry_t *new_entry = calloc(1, sizeof(map_entry_t));
	new_entry->key = key;
	new_entry->next = bucket->first;
	bucket->first = new_entry;
	return &new_entry->value;
}

static uint64_t map_bucket_remove(map_t *map, map_bucket_t *bucket, uint64_t key)
{
	map_entry_t *entry;
	for (entry = bucket->first; entry; entry = entry->next)
	{
		if (entry->key == key)
		{
			uint64_t value = entry->value;
			map_entry_t **next_holder = &bucket->first;
			while (*next_holder != entry)
			{
				next_holder = &(*next_holder)->next;
			}
			*next_holder = entry->next;
			free(entry);
			map->count--;
			return value;
		}
	}

	return 0;
}

uint64_t *map_get(map_t const *map, uint64_t key)
{
	return bucket_get_value(map_get_bucket(map, key), key);
}

void *map_get_deref(const map_t *map, uint64_t key)
{
	uint64_t *ptr = bucket_get_value(map_get_bucket(map, key), key);
	if (ptr)
	{
		return (void *)(uintptr_t)ptr[0];
	}
	return NULL;
}

void map_init(map_t *map)
{
	memset(map, 0, sizeof(map_t));
	map_rehash(map, 0);
}

void map_insert(map_t *map, uint64_t key, uint64_t value)
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
	bucket_add(bucket, key)[0] = value;
}

void *map_insert_alloc(map_t *map, int elem_size, uint64_t key)
{
	void *elem = calloc(1, elem_size);
	map_insert(map, key, (uintptr_t)elem);
	return elem;
}

uint64_t *map_ensure(map_t *map, uint64_t key)
{
	map_bucket_t *bucket = map_get_bucket(map, key);
	uint64_t *result = bucket_get_value(bucket, key);
	if (result)
	{
		return result;
	}

	int32_t map_count = ++map->count;
	int32_t tgt_bucket_count = map_get_bucket_count(map_count);
	int32_t bucket_count = map->bucket_count;
	if (tgt_bucket_count > bucket_count)
	{
		map_rehash(map, tgt_bucket_count);
		bucket = map_get_bucket(map, key);
	}

	uint64_t *v = bucket_add(bucket, key);
	*v = 0;
	return v;
}

void *map_ensure_alloc(map_t *map, int elem_size, uint64_t key)
{
	uint64_t *val = map_ensure(map, key);
	if ((*val) == 0)
	{
		void *elem = calloc(1, elem_size);
		*val = (uint64_t)(uintptr_t)elem;
		return elem;
	}
	else
	{
		return (void *)(uintptr_t)*val;
	}
}

uint64_t map_remove(map_t *map, uint64_t key)
{
	return map_bucket_remove(map, map_get_bucket(map, key), key);
}

map_iter_t map_iter(const map_t *map)
{
	if (map->bucket_shift != 0)
	{
		return (map_iter_t){
			.map = map,
			.bucket = NULL,
			.entry = NULL};
	}
	else
	{
		return (map_iter_t){0};
	}
}

int map_next(map_iter_t *iter)
{
	const map_t *map = iter->map;
	map_bucket_t *end;
	if (!map || (iter->bucket == (end = &map->buckets[map->bucket_count])))
	{
		return 0;
	}

	map_entry_t *entry = NULL;
	if (!iter->bucket)
	{
		for (iter->bucket = map->buckets; iter->bucket != end; ++iter->bucket)
		{
			if (iter->bucket->first)
			{
				entry = iter->bucket->first;
				break;
			}
		}
		if (iter->bucket == end)
		{
			return 0;
		}
	}
	else if ((entry = iter->entry) == NULL)
	{
		do
		{
			++iter->bucket;
			if (iter->bucket == end)
			{
				return 0;
			}
		} while (!iter->bucket->first);
		entry = iter->bucket->first;
	}

	lwmath_assert(entry != NULL, "");
	iter->entry = entry->next;
	iter->res = &entry->key;

	return 1;
}

static void bucket_clear(map_bucket_t *bucket)
{
	map_entry_t *entry = bucket->first;
	while (entry)
	{
		map_entry_t *next = entry->next;
		free(entry);
		entry = next;
	}
}

void map_fini(map_t *map)
{
	if (map->bucket_shift == 0)
	{
		return;
	}

	map_bucket_t *bucket = map->buckets;
	map_bucket_t *end = &bucket[map->bucket_count];
	while (bucket != end)
	{
		bucket_clear(bucket);
		bucket++;
	}

	free(map->buckets);

	//map->bucket_shift = 0;
	memset(map, 0, sizeof(map_t));
}