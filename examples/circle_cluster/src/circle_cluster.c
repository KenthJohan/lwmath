#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>

#include <lwmath/lin.h>
#include <lwmath/cclab.h>
#include <lwmath/map.h>

#include <tigr/tigr.h>
#include <tigr/tigr_mouse.h>

#include <flecs.h>


#define MAX_POINTS 128

void connected_components_demo(uint32_t *input, uint32_t *comp, uint32_t *labels, Tigr *out)
{
	TPixel *pix = out->pix;
	int n = out->w * out->h;
	for (int i = 0; i < n; ++i, ++pix)
	{
		uint32_t c = labels[i];
		if (c)
		{
			uint32_t h = djb33_hash((char *)&c, sizeof(uint32_t));
			pix->r = (h >> 0) & 0xFF;
			pix->g = (h >> 8) & 0xFF;
			pix->b = (h >> 16) & 0xFF;
			pix->a = 0xFF;
		}
		else
		{
			pix->r = 0x00;
			pix->g = 0x00;
			pix->b = 0x00;
			pix->a = 0xFF;
		}
	}
}



#define THE_FLAG 0x01

void put_flag_by_mouse(uint32_t *flags, int w, int h, tigr_mouse_t *mouse)
{
	if (mouse->down)
	{
		int x = mouse->x;
		int y = mouse->y;
		int i = y * w + x;
		flags[i] ^= THE_FLAG;
	}
}


typedef struct
{
	map_entry_t entry;
	vec2_t buf[0];
} region_t;


void labels_mapping(uint32_t *labels, uint32_t *components, int w, int h, map_t * map)
{
	int j = 0;
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int i = y * w + x;
			uint32_t key = labels[i];
			if (key > 0)
			{
				uint32_t n = components[i];
				region_t * region = calloc(1, sizeof(region_t) + sizeof(vec2_t)*n);
				map_insert(map, key, &region->entry);

				//vec2_t * ptr = map_ensure_alloc(map, sizeof(vec2_t)*n, key);

			}
		}
	}
}


typedef struct
{
	tigr_mouse_t mouse;
	int clist_count;
} app_t;

int main(int argc, char *argv[])
{
	ecs_world_t *world = ecs_init();
	app_t app = {0};
	int w = 100;
	int h = 100;
	Tigr *bmp_screen = tigrWindow(w, h, "circle_fitting", 0);
	Tigr *bmp_paint = tigrBitmap(w, h);
	Tigr *bmp_labels = tigrBitmap(w, h);

	uint32_t *flags = calloc(1, sizeof(uint32_t) * w * h);
	uint32_t *labels = calloc(1, sizeof(uint32_t) * w * h);
	uint32_t *components = calloc(1, sizeof(uint32_t) * w * h);
	map_t map = {0};
	map_init(&map);

	while (!tigrClosed(bmp_screen))
	{
		tigrClear(bmp_screen, tigrRGB(0x00, 0x00, 0x00));
		// tigrCircle(screen, 50, 50, 10, tigrRGB(0xFF, 0xFF, 0xFF));
		tigr_mouse_get(bmp_screen, &app.mouse);

		put_flag_by_mouse(flags, w, h, &app.mouse);

		cclab_union_find(flags, THE_FLAG, components, labels, w, h, 1);
		labels_mapping(labels, w, h, &map);
		
		connected_components_demo(flags, components, labels, bmp_labels);

		tigrBlitAlpha(bmp_screen, bmp_labels, 0, 0, 0, 0, w, h, 0.5f);
		tigrBlitAlpha(bmp_screen, bmp_paint, 0, 0, 0, 0, w, h, 0.5f);

		tigrUpdate(bmp_screen);

		if (app.mouse.down)
		{
			printf("clist_count %i\n", app.clist_count);
		}
	}
	tigrFree(bmp_screen);
	tigrFree(bmp_paint);
	tigrFree(bmp_labels);
	
	ecs_fini(world);
	return 0;
}