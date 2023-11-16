#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include <assert.h>
#include <math.h>

#include <lwmath/lin.h>
#include <lwmath/cclab.h>
#include <lwmath/fitcirc.h>
#include <lwmath/map.h>

#include <tigr/tigr.h>
#include <tigr/tigr_mouse.h>




#define MAX_POINTS 128

void paint_labels(uint32_t *input, uint32_t *comp, uint32_t *labels, Tigr *out)
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
	int x = mouse->x;
	int y = mouse->y;
	int i = y * w + x;
	if(mouse->down)
	{
		//flags[i] ^= THE_FLAG;
	}

	if(mouse->btn & 0x1)
	{
		flags[i] |= THE_FLAG;
	}
	if(mouse->btn & 0x2)
	{
		flags[i] &= ~THE_FLAG;
	}
	//printf("%i\n", flags[i]);
}


typedef struct
{
	map_entry_t entry;
	float a;
	float b;
	float r;
	float e;
	float eqm1;
	float eqm2;
	float eqmO;
	int cap;
	int count;
	vec2_t buf[];
} region_t;

region_t * region_map_ensure(map_t * map, uint64_t key, int cap)
{
	region_t * region = map_get_t(map, key, region_t, entry);
	if(region == NULL)
	{
		region = calloc(1, sizeof(region_t) + sizeof(vec2_t)*cap);
		region->cap = cap;
		region->count = 0;
		map_insert(map, key, &region->entry);
	}
	return region;
}

void labels_mapping(uint32_t *labels, uint32_t *components, int w, int h, map_t * map)
{
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int i = y * w + x;
			uint32_t key = labels[i];
			if (key > 0)
			{
				uint32_t n = components[key];
				region_t * region = region_map_ensure(map, key, n);
				assert(region->count < region->cap);
				region->buf[region->count].x = x;
				region->buf[region->count].y = y;
				region->count++;
			}
		}
	}
}

void regions_flush(map_t * map)
{
	map_iter_t it = map_iter(map);
	while(1)
	{
		map_entry_t * e = map_next(&it);
		if(e == NULL)
		{
			break;
		}
		region_t * r = map_entry(e, region_t, entry);
		free(r);
	}
	map_clear(map);
}

void paint_region(region_t * region, Tigr *out)
{
	float x = region->a;
	float y = region->b;
	float r = region->r;

	//if(x <= 0){return;}
	//if(x >= w){return;}
	//if(y <= 0){return;}
	//if(y >= h){return;}
	//if(r <= 0){return;}
	//if(r > 10000){return;}
	//printf("fitcirc %f %f %f\n", x, y, r);
	tigrCircle(out, round(x), round(y), round(r), tigrRGBA(0xFF, 0xFF, 0xFF, 0xAA));
	char buf[256];
	snprintf(buf, sizeof(buf), "%f\n%f", r, region->eqmO);
	tigrPrint(out, tfont, x+r+10, y-r-10, tigrRGBA(0xc0, 0xd0, 0xff, 0xFF), buf);
}


void circle1(map_t * map, Tigr *out)
{
	map_iter_t it = map_iter(map);
	while(1)
	{
		map_entry_t * e = map_next(&it);
		if(e == NULL)
		{
			break;
		}
		region_t * region = map_entry(e, region_t, entry);

		for(int i = 0; i < region->count; ++i)
		{
			float x = region->buf[i].x;
			float y = region->buf[i].y;
			tigrPlot(out, x, y, tigrRGBA(0xFF, 0xd0, 0x22, 0xFF));
		}

		fitcirc(region->buf, region->count, sizeof(vec2_t), &region->a, &region->b, &region->r);
		if(!isnormal(region->a)) {continue;}
		if(!isnormal(region->b)) {continue;}
		if(!isnormal(region->r)) {continue;}

		for(int i = 0; i < region->count; ++i)
		{
			float x = region->buf[i].x;
			float y = region->buf[i].y;
			float dx = x - region->a;
			float dy = y - region->b;
			float l = sqrtf(dx*dx + dy*dy);
			if(l > region->r)
			{
				tigrPlot(out, x, y, tigrRGBA(0xFF, 0x00, 0x00, 0xFF));
			}
			else
			{
				tigrPlot(out, x, y, tigrRGBA(0x00, 0x00, 0xFF, 0xFF));
			}
		}

		//region->eqm1 = fitcirc_error(region->buf, region->count, sizeof(vec2_t), region->a, region->b, region->r, FITCIRC_EFN_EQM);
		//region->eqm2 = fitcirc_error(region->buf, region->count, sizeof(vec2_t), region->a, region->b, region->r, FITCIRC_EFN_SIGNED);
		region->eqmO = fitcirc_error(region->buf, region->count, sizeof(vec2_t), region->a, region->b, region->r, FITCIRC_EFN_EQM);
		paint_region(region, out);



	}
}





typedef struct
{
	tigr_mouse_t mouse;
	int clist_count;
} app_t;

int main(int argc, char *argv[])
{
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
	map_init(&map, 10);

	while (!tigrClosed(bmp_screen))
	{
		// tigrCircle(screen, 50, 50, 10, tigrRGB(0xFF, 0xFF, 0xFF));
		tigr_mouse_get(bmp_screen, &app.mouse);

		if (app.mouse.btn)
		{
			tigrClear(bmp_paint, tigrRGBA(0x00, 0x00, 0x00, 0x00));
			put_flag_by_mouse(flags, w, h, &app.mouse);
			cclab_union_find(flags, THE_FLAG, components, labels, w, h, 1);
			labels_mapping(labels, components, w, h, &map);
			printf("map.count %i\n", map.count);
			circle1(&map, bmp_paint);
			regions_flush(&map);

			//paint_labels(flags, components, labels, bmp_labels);
		}

		tigrClear(bmp_screen, tigrRGB(0x00, 0x00, 0x00));
		tigrBlitAlpha(bmp_screen, bmp_labels, 0, 0, 0, 0, w, h, 1.0f);
		tigrBlitAlpha(bmp_screen, bmp_paint, 0, 0, 0, 0, w, h, 0.5f);
		tigrUpdate(bmp_screen);

	}
	tigrFree(bmp_screen);
	tigrFree(bmp_paint);
	tigrFree(bmp_labels);
	

	return 0;
}