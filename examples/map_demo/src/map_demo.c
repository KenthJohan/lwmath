#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include <assert.h>

#include <lwmath/map.h>


typedef struct 
{
	map_entry_t entry;
	int data1;
	int data2;
} payload_t;


void print_map(map_t * map)
{
	map_iter_t it = map_iter(map);
	while(1)
	{
		map_entry_t * entry = map_next(&it);
		if(entry == NULL)
		{
			break;
		}
		printf("%10ju\n", (uintmax_t)entry->key);
	}
}



int main(int argc, char *argv[])
{
	map_t map = {0};
	map_init(&map);

	payload_t * p1 = calloc(1, sizeof(payload_t));
	payload_t * p2 = calloc(1, sizeof(payload_t));
	payload_t * p3 = calloc(1, sizeof(payload_t));
	map_insert(&map, 100, &p1->entry);
	map_insert(&map, 101, &p2->entry);
	map_insert(&map, 2000, &p3->entry);

	printf("Should have 3 items:\n");
	print_map(&map);

	payload_t * p3_hello = map_remove_t(&map, 2000, payload_t, entry);

	printf("Should have 2 items:\n");
	print_map(&map);



	/*
	map_insert(&map, 101, 2);
	map_insert(&map, 4576457, 2);
	map_insert(&map, 4545457, 2);
	map_insert(&map, 6786787667, 2);
	map_insert(&map, 465756457, 2);

	uint64_t * value1 = map_ensure(&map, 100);
	assert(*value1 == 1);

	uint64_t * value2 = map_ensure(&map, 42);
	assert(*value2 == 0);

	uint64_t value3 = map_remove(&map, 100);
	assert(value3 == 1);

	uint64_t * value4 = map_ensure(&map, 100);
	assert(*value4 == 0);

	map_iter_t it = map_iter(&map);
	while(map_next(&it))
	{
		printf("%10ju : %10ju\n", (uintmax_t)map_key(&it), (uintmax_t)map_value(&it));
	}
	map_fini(&map);
	*/

	return 0;
}