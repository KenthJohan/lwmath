#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include <flecs.h>
#include <lwmath/lin.h>

#include <tigr/tigr.h>

void circle1(Tigr const *bmp, Tigr *out)
{
	int j = 0;
	vec2_t pos[128];
	for (int x = 0; x < bmp->w; ++x)
	{
		for (int y = 0; y < bmp->h; ++y)
		{
			int i = y * bmp->w + x;
			if (bmp->pix[i].r == 0xFF)
			{
				if (j > 128)
				{
					goto next;
				}
				pos[j].x = x;
				pos[j].y = y;
				j++;
			}
		}
	}
	float a;
	float b;
	float r;
next:
	circfit((float *)pos, j, 2, &a, &b, &r);
	if(a >= 0 && a < bmp->w && b >= 0 && b < bmp->h && r > 0)
	{
		tigrCircle(out, a, b, r, tigrRGB(0xFF, 0xFF, 0xFF));
	}
	return;
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
		circle1(backdrop, screen);
		tigrBlitAlpha(screen, backdrop, 0, 0, 0, 0, backdrop->w, backdrop->h, 0.5f);

		tigrUpdate(screen);
	}
	tigrFree(screen);
	tigrFree(backdrop);

	ecs_fini(world);
	return 0;
}