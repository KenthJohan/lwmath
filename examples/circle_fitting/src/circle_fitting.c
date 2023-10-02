#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <lwmath/lin.h>
#include <lwmath/fitcirc.h>

#include <tigr/tigr.h>
#include <tigr/tigr_mouse.h>

#define THE_FLAG 0x01

void circle1(uint32_t const *flags, Tigr *out)
{
	int j = 0;
	vec2_t pos[128];
	int w = out->w;
	int h = out->h;
	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			int i = y * w + x;
			if (flags[i] & THE_FLAG)
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
	fitcirc(pos, j, sizeof(vec2_t), &a, &b, &r);
	if(a >= 0 && a < w && b >= 0 && b < h && r > 0)
	{
		float e = fitcirc_error2(pos, j, sizeof(vec2_t), a, b, r);
		char buf[64];
		snprintf(buf, 64, "Error %f\n", e);
		//printf("error: %f\n", e);
		tigrPrint(out, tfont, 0, 0, tigrRGB(0xFF, 0xFF, 0x00), buf);
		tigrCircle(out, round(a), round(b), round(r), tigrRGB(0xFF, 0xFF, 0xFF));
	}
	return;
}


void paint_flags(uint32_t *flags, Tigr *out)
{
	TPixel *pix = out->pix;
	int n = out->w * out->h;
	for (int i = 0; i < n; ++i, ++pix)
	{
		uint32_t c = flags[i];
		if (c & THE_FLAG)
		{
			pix->r = 0xFF;
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




void put_flag_by_mouse(uint32_t *flags, int w, int h, tigr_mouse_t *mouse)
{
	int x = mouse->x;
	int y = mouse->y;
	int i = y * w + x;
	if(mouse->down)
	{
		flags[i] ^= THE_FLAG;
	}

	if(mouse->btn & 0x1)
	{
		//flags[i] |= THE_FLAG;
	}
	if(mouse->btn & 0x2)
	{
		//flags[i] &= ~THE_FLAG;
	}
	//printf("%i\n", flags[i]);
}


int main(int argc, char *argv[])
{
	app_t app = {0};
	int w = 100;
	int h = 100;
	Tigr *bmp_screen = tigrWindow(w, h, "circle_fitting", 0);
	Tigr *bmp_paint = tigrBitmap(w, h);
	Tigr *bmp_circle = tigrBitmap(w, h);
	uint32_t *flags = calloc(1, sizeof(uint32_t) * w * h);
	while (!tigrClosed(bmp_screen))
	{
		//tigrCircle(screen, 50, 50, 10, tigrRGB(0xFF, 0xFF, 0xFF));
		tigr_mouse_get(bmp_screen, &app.mouse);
		if(app.mouse.down)
		{
			tigrClear(bmp_screen, tigrRGB(0x00, 0x00, 0x00));
			tigrClear(bmp_circle, tigrRGB(0x00, 0x00, 0x00));
			tigrClear(bmp_paint, tigrRGB(0x00, 0x00, 0x00));
			put_flag_by_mouse(flags, w, h, &app.mouse);
			paint_flags(flags, bmp_paint);
			circle1(flags, bmp_circle);
			tigrBlitAlpha(bmp_screen, bmp_circle, 0, 0, 0, 0, w, h, 1.0f);
			tigrBlitAlpha(bmp_screen, bmp_paint, 0, 0, 0, 0, w, h, 0.6f);
		}
		tigrUpdate(bmp_screen);

	}
	tigrFree(bmp_screen);
	tigrFree(bmp_paint);
	tigrFree(bmp_circle);

	return 0;
}