#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <platform/net.h>
#include <platform/log.h>
#include <platform/fs.h>
#include <platform/assert.h>

#include <hac/hac_types.h>
#include <hac/hac.h>
#include <hac/sensor.h>
#include <hac/hac_math.h>
#include <hac/cclab.h>


#include <flecs.h>
#include <float.h>

#include <tigr/tigr.h>


void connected_components_demo(Tigr const *bmp, uint32_t * comp, uint32_t * labels, Tigr *out)
{
	cclab_union_find((uint32_t*)bmp->pix, 0xFFFFFFFF, comp, labels, bmp->w, bmp->h);
	TPixel * pix = out->pix;
	for(int i = 0; i < (bmp->w * bmp->h); ++i, ++pix)
	{
		uint32_t c = labels[i];
		if(c)
		{
			uint32_t h = djb33_hash((char*)&c, sizeof(uint32_t));
			pix->r = (h >> 0) & 0xFF;
			pix->g = (h >> 8) & 0xFF;
			pix->b = (h >> 16) & 0xFF;
			pix->a = 255;
		}
	}
}

typedef struct
{
	int mouse_btn;
	int mouse_up;
	int mouse_down;
	int mouse_x;
	int mouse_y;
} app_t;


void paint(Tigr *bmp, app_t * app)
{
	if(app->mouse_down)
	{
		int x = app->mouse_x;
		int y = app->mouse_y;
		int i = y * bmp->w + x;
		bmp->pix[i].r = 0xFF;
		bmp->pix[i].g = 0x00;
		bmp->pix[i].b = 0x00;
		bmp->pix[i].a = 0xFF;
	}
}

int main(int argc, char *argv[])
{
	ecs_world_t *world = ecs_init();
	app_t app = {0};
	Tigr *screen = tigrWindow(100, 100, "circle_fitting", 0);
	Tigr *backdrop = tigrBitmap(screen->w, screen->h);

	uint32_t * labels = calloc(1, sizeof(uint32_t)*backdrop->w*backdrop->h);
	uint32_t * components = calloc(1, sizeof(uint32_t)*backdrop->w*backdrop->h);

	while (!tigrClosed(screen))
	{
		tigrClear(screen, tigrRGB(0x00, 0x00, 0x00));
		//tigrCircle(screen, 50, 50, 10, tigrRGB(0xFF, 0xFF, 0xFF));

		{
			int b;
			tigrMouse(screen, &app.mouse_x, &app.mouse_y, &b);
			app.mouse_up = (app.mouse_btn ^ b) & ~b;
			app.mouse_down = (app.mouse_btn ^ b) & b;
			app.mouse_btn = b;
		}

		paint(backdrop, &app);
		connected_components_demo(backdrop, components, labels, backdrop);
		tigrBlitAlpha(screen, backdrop, 0, 0, 0, 0, backdrop->w, backdrop->h, 0.5f);

		tigrUpdate(screen);
	}
	tigrFree(screen);
	tigrFree(backdrop);

	ecs_fini(world);
	return 0;
}