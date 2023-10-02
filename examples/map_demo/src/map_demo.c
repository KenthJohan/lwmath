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
	map_init(&map, 5);

	payload_t * p1 = calloc(1, sizeof(payload_t));
	payload_t * p2 = calloc(1, sizeof(payload_t));
	payload_t * p3 = calloc(1, sizeof(payload_t));
	payload_t * p4 = calloc(1, sizeof(payload_t));

	map_insert(&map, 100, &p1->entry);
	map_insert(&map, 101, &p2->entry);
	map_insert(&map, 2000, &p3->entry);
	map_insert(&map, 0, &p4->entry);
	assert(map.count == 4);
	assert(map_get_t(&map, 100, payload_t, entry) == p1);
	assert(map_get_t(&map, 101, payload_t, entry) == p2);
	assert(map_get_t(&map, 2000, payload_t, entry) == p3);
	assert(map_get_t(&map, 0, payload_t, entry) == p4);


	printf("Should have 4 items:\n");
	print_map(&map);

	payload_t * p3_hello = map_remove_t(&map, 2000, payload_t, entry);
	assert(p3_hello == p3);
	assert(map.count == 3);

	printf("Should have 3 items:\n");
	print_map(&map);

	free(p1);
	free(p2);
	free(p3);
	free(p4);

	map_fini(&map);
	return 0;
}