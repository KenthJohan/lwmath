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
	tigr_mouse_t mouse;
} app_t;


void paint(Tigr *bmp, app_t * app)
{
	if(app->mouse.down)
	{
		int x = app->mouse.x;
		int y = app->mouse.y;
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
		tigr_mouse_get(screen, &app.mouse);

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