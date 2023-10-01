#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>

#include <lwmath/lin.h>
#include <lwmath/cclab.h>

#include <tigr/tigr.h>
#include <tigr/tigr_mouse.h>

#include <flecs.h>



void connected_components_demo(uint32_t * input, uint32_t * comp, uint32_t * labels, Tigr *out)
{
	TPixel * pix = out->pix;
	int n = out->w * out->h;
	for(int i = 0; i < n; ++i, ++pix)
	{
		uint32_t c = labels[i];
		if(c)
		{
			uint32_t h = djb33_hash((char*)&c, sizeof(uint32_t));
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

typedef struct
{
	tigr_mouse_t mouse;
} app_t;

#define THE_FLAG 0x01

void put_flag_by_mouse(uint32_t * flags, int w, int h, tigr_mouse_t * mouse)
{
	if(mouse->down)
	{
		int x = mouse->x;
		int y = mouse->y;
		int i = y * w + x;
		flags[i] ^= THE_FLAG;
	}
}

int main(int argc, char *argv[])
{
	ecs_world_t *world = ecs_init();
	app_t app = {0};
	int w = 100;
	int h = 100;
	Tigr *bmp_screen = tigrWindow(w, h, "circle_fitting", 0);
	Tigr *bmp_paint = tigrBitmap(w, h);
	Tigr *bmp_labels = tigrBitmap(w, h);

	uint32_t * flags = calloc(1, sizeof(uint32_t)*w*h);
	uint32_t * labels = calloc(1, sizeof(uint32_t)*w*h);
	uint32_t * components = calloc(1, sizeof(uint32_t)*w*h);

	while (!tigrClosed(bmp_screen))
	{
		tigrClear(bmp_screen, tigrRGB(0x00, 0x00, 0x00));
		//tigrCircle(screen, 50, 50, 10, tigrRGB(0xFF, 0xFF, 0xFF));
		tigr_mouse_get(bmp_screen, &app.mouse);

		put_flag_by_mouse(flags, w, h, &app.mouse);


		cclab_union_find(flags, THE_FLAG, components, labels, w, h, 1);
		connected_components_demo(flags, components, labels, bmp_labels);


		tigrBlitAlpha(bmp_screen, bmp_labels, 0, 0, 0, 0, w, h, 0.5f);
		tigrBlitAlpha(bmp_screen, bmp_paint, 0, 0, 0, 0, w, h, 0.5f);

		tigrUpdate(bmp_screen);
	}
	tigrFree(bmp_screen);
	tigrFree(bmp_paint);
	tigrFree(bmp_labels);

	ecs_fini(world);
	return 0;
}