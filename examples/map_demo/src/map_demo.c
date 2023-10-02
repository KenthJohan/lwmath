#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include <assert.h>

#include <lwmath/map.h>


int main(int argc, char *argv[])
{
	map_t map = {0};
	map_init(&map);
	map_insert(&map, 100, 1);
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

	return 0;
}