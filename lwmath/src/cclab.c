#include "lwmath/cclab.h"
#include "lwmath_assert.h"
#include <string.h>

/*
https://stackoverflow.com/questions/14465297/connected-component-labeling-implementation

const int w = 5, h = 5;
int input[w][h] =  {{1,0,0,0,1},
					{1,1,0,1,1},
					{0,1,0,0,1},
					{1,1,1,1,0},
					{0,0,0,1,0}};
int component[w*h];

void doUnion(int a, int b)
{
	// get the root component of a and b, and set the one's parent to the other
	while (component[a] != a)
		a = component[a];
	while (component[b] != b)
		b = component[b];
	component[b] = a;
}

void unionCoords(int x, int y, int x2, int y2)
{
	if (y2 < h && x2 < w && input[x][y] && input[x2][y2])
		doUnion(x*h + y, x2*h + y2);
}

int main()
{
	for (int i = 0; i < w*h; i++)
		component[i] = i;
	for (int x = 0; x < w; x++)
	for (int y = 0; y < h; y++)
	{
		unionCoords(x, y, x+1, y);
		unionCoords(x, y, x, y+1);
	}

	// print the array
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			if (input[x][y] == 0)
			{
				cout << ' ';
				continue;
			}
			int c = x*h + y;
			while (component[c] != c) c = component[c];
			cout << (char)('a'+c);
		}
		cout << "\n";
	}
}

*/


uint32_t traverse_to_root(uint32_t *component, uint32_t a)
{
	lwmath_assert_notnull(component);
	while (component[a] != a)
	{
		a = component[a];
	}
	return a;
}


void cclab_do_union(uint32_t *component, uint32_t a, uint32_t b)
{
	lwmath_assert_notnull(component);
	// get the root component of a and b, and set the one's parent to the other
	a = traverse_to_root(component, a);
	b = traverse_to_root(component, b);
	component[b] = a;
}


void cclab_union_coords(uint32_t const *input, uint32_t mask, uint32_t *component, uint32_t w, uint32_t h, uint32_t x, uint32_t y, uint32_t x2, uint32_t y2)
{
	lwmath_assert_notnull(input);
	lwmath_assert_notnull(component);
	lwmath_assert(x < w, "Not in range");
	lwmath_assert(y < h, "Not in range");
	lwmath_assert(x2 < w, "Not in range");
	lwmath_assert(y2 < h, "Not in range");

	uint32_t i1 = y * w + x;
	uint32_t i2 = y2 * w + x2;
	uint32_t p1 = input[i1] & mask;
	uint32_t p2 = input[i2] & mask;
	if (p1 && p2)
	{
		cclab_do_union(component, i1, i2);
	}
}


void cclab_count(uint32_t *input, uint32_t * hgram, uint32_t n)
{
	lwmath_assert_notnull(input);
	lwmath_assert_notnull(hgram);

	memset(hgram, 0, sizeof(uint32_t)*n);
	for (uint32_t i = 0; i < n; ++i)
	{
		uint32_t a = input[i];
		if (a)
		{
			hgram[a]++;
		}
	}
}


void cclab_union_find(uint32_t const *input, uint32_t mask, uint32_t *component, uint32_t *labels, uint32_t w, uint32_t h)
{
	lwmath_assert_notnull(input);
	lwmath_assert_notnull(component);
	lwmath_assert_notnull(labels);

	for (uint32_t i = 0; i < w * h; ++i)
	{
		labels[i] = 0;
		component[i] = i;
	}

	for (uint32_t x = 0; x < w-1; x++)
	{
		for (uint32_t y = 0; y < h-1; y++)
		{
			cclab_union_coords(input, mask, component, w, h, x, y, (x + 0), (y + 1));
			cclab_union_coords(input, mask, component, w, h, x, y, (x + 1), (y + 0));
			//cclab_union_coords(input, mask, component, w, h, x, y, x + 1, y + 1);
		}
	}

	for (uint32_t y = 0; y < h; ++y)
	{
		for (uint32_t x = 0; x < w; ++x)
		{
			uint32_t i = y * w + x;
			if ((input[i] & mask) == 0)
			{
				continue;
			}
			uint32_t j = traverse_to_root(component, i);
			labels[i] = j;
		}
	}


	cclab_count(labels, component, w*h);

	/*
	int mini;
	int maxi;
	find_minmax_u32_index(component, w*h, &mini, &maxi);

	for (uint32_t i = 0; i < w * h; ++i)
	{
		uint32_t a = labels[i];
		if(component[a] < 4)
		{
			labels[i] = 0;
		}
	}
	*/

}
