#include "lwmath/fitcirc.h"
#include "lwmath/lin.h"
#include <stdio.h>
#include <math.h>

/*
https://www.scribd.com/document/14819165/Regressions-coniques-quadriques-circulaire-spherique
page 11 to 15
*/
void fitcirc(void * data, int n, int stride, float * out_a, float * out_b, float * out_r)
{
	float sum_x = 0;
	float sum_y = 0;
	float sum_xx = 0;
	float sum_xy = 0;
	float sum_yy = 0;
	float sum_xxx = 0;
	float sum_xxy = 0;
	float sum_xyy = 0;
	float sum_yyy = 0;
	char * ptr = data;
	for (int i = 0; i < n; ++i, ptr += stride)
	{
		float * p = (float *)ptr;
		float x = p[0];
		float y = p[1];
		sum_x += x;
		sum_y += y;
		sum_xx += x*x;
		sum_xy += x*y;
		sum_yy += y*y;
		sum_xxx += x*x*x;
		sum_xxy += x*x*y;
		sum_xyy += x*y*y;
		sum_yyy += y*y*y;
	}

	/*
	printf("  x S10 %f  = 6713\n", sum_x);
	printf("  y S01 %f  = 42591\n", sum_y);
	printf(" yy S02 %E  = 1.68628E6\n", sum_yy);
	printf(" xy S11 %E  = 8.44326E6\n", sum_xy);
	printf(" xx S20 %E  = 6.14944E7\n", sum_xx);
	printf("yyy S03 %E  = 4.82961E8\n", sum_yyy);
	printf("xyy S12 %E  = 2.13381E9\n", sum_xyy);
	printf("xxy S21 %.6E  = 1.18558E10\n", sum_xxy);
	printf("xxx S30 %.6E  = 9.8881E10\n\n", sum_xxx);
	*/

	// dans lequel on a posé, pour simplifier les écritures :
	float d11 = (n * sum_xy) - (sum_x * sum_y);
	float d20 = (n * sum_xx) - (sum_x * sum_x);
	float d30 = (n * sum_xxx) - (sum_xx * sum_x);
	float d21 = (n * sum_xxy) - (sum_xx * sum_y);
	float d02 = (n * sum_yy) - (sum_y * sum_y);
	float d03 = (n * sum_yyy) - (sum_y * sum_yy);
	float d12 = (n * sum_xyy) - (sum_x * sum_yy);

	/*
	printf("d02 %E  = 1.39554E7\n", d02);
	printf("d11 %E  = 9.60068E6\n", d11);
	printf("d20 %E  = 3.38311E8\n", d20);
	printf("d03 %E  = 5.58365E9\n", d03);
	printf("d12 %E  = 2.86298E9\n", d12);
	printf("d21 %E  = 2.13941E9\n", d21);
	printf("d30 %E  = 8.41728E11\n", d30);
	*/

	float h = 2.0 * ((d20 * d02) - (d11*d11));
	// La résolution du système de deux équations linéaires donne les cordonnées (a,b) du centre du cercle :
	float a = (((d30 + d12) * d02) - ((d03 + d21) * d11)) / h;
	float b = (((d03 + d21) * d20) - ((d30 + d12) * d11)) / h;

	// Puis, en revenant à la 3ième équation du système initial, on obtient le rayon du cercle :
	float c = (1.0f / (float)n) * ((sum_xx + sum_yy) - (2.0f * a * sum_x) - (2.0f * b * sum_y));
	float r = sqrtf(c + a*a + b*b);

	out_a[0] = a;
	out_b[0] = b;
	out_r[0] = r;
}

float fitcirc_error(float x, float y, float a, float b, float r)
{
	float xa = x - a;
	float yb = y - b;
	return r - sqrtf(xa*xa + yb*yb);
}


float fitcirc_error2(void * data, int n, int stride, float a, float b, float r)
{
	char * ptr = data;
	float sum = 0;
	for (int i = 0; i < n; ++i, ptr += stride)
	{
		float * p = (float *)ptr;
		float x = p[0];
		float y = p[1];
		float e = fitcirc_error(x, y, a, b, r);
		sum += e*e;
	}
	sum = (1.0 / (float)n) * sum;
	return sqrtf(sum);
}






/*

void tigrPlot(uint32_t* bmp, int x, int y, uint32_t pix) {
    int xa, i, a;

    int cx = bmp->cx;
    int cy = bmp->cy;
    int cw = bmp->cw >= 0 ? bmp->cw : bmp->w;
    int ch = bmp->ch >= 0 ? bmp->ch : bmp->h;

    if (x >= cx && y >= cy && x < cx + cw && y < cy + ch) {
        xa = EXPAND(pix.a);
        a = xa * xa;
        i = y * bmp->w + x;

        bmp->pix[i].r += (unsigned char)((pix.r - bmp->pix[i].r) * a >> 16);
        bmp->pix[i].g += (unsigned char)((pix.g - bmp->pix[i].g) * a >> 16);
        bmp->pix[i].b += (unsigned char)((pix.b - bmp->pix[i].b) * a >> 16);
        bmp->pix[i].a += (bmp->blitMode) * (unsigned char)((pix.a - bmp->pix[i].a) * a >> 16);
    }
}


void tigrLine(Tigr* bmp, int x0, int y0, int x1, int y1, TPixel color) {
    int sx, sy, dx, dy, err, e2;
    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    if (x0 < x1)
        sx = 1;
    else
        sx = -1;
    if (y0 < y1)
        sy = 1;
    else
        sy = -1;
    err = dx - dy;

    do {
        tigrPlot(bmp, x0, y0, color);
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    } while (x0 != x1 || y0 != y1);
}


void tigrFillCircle(Tigr* bmp, int x0, int y0, int r, TPixel color) {
    if (r <= 0) {
        return;
    }

    int E = 1 - r;
    int dx = 0;
    int dy = -2 * r;
    int x = 0;
    int y = r;

    tigrLine(bmp, x0 - r + 1, y0, x0 + r, y0, color);

    while (x < y - 1) {
        x++;

        if (E >= 0) {
            y--;
            dy += 2;
            E += dy;
            tigrLine(bmp, x0 - x + 1, y0 + y, x0 + x, y0 + y, color);
            tigrLine(bmp, x0 - x + 1, y0 - y, x0 + x, y0 - y, color);
        }

        dx += 2;
        E += dx + 1;

        if (x != y) {
            tigrLine(bmp, x0 - y + 1, y0 + x, x0 + y, y0 + x, color);
            tigrLine(bmp, x0 - y + 1, y0 - x, x0 + y, y0 - x, color);
        }
    }
}


*/





















void test()
{
	vec2_t p[] = {
	{1957.3, 60.27},
	{1941.6, 76.56},
	{1932.0, 86.06},
	{1915.7, 101.7},
	{1902.6, 113.8},
	{1886.7, 127.7},
	{1863.9, 146.8},
	{1829.0, 173.7},
	{1786.3, 203.3},
	{1747.3, 227.5},
	{1678.2, 264.3},
	{1625.0, 288},
	{1582.7, 304.1},
	{1507.3, 327.3},
	{1421.1, 345.8},
	{1326.2, 356.7},
	{1253.0, 358.5},
	{1146.8, 351.3},
	{1075.7, 339.6},
	{1028.1, 328.7},
	{955.11, 306.8},
	{881.0, 277.7},
	{812.45, 244.2},
	{769.8, 219.7},
	{717.9, 185.7},
	{687.45, 163.4},
	{666.82, 147.2},
	{640.7, 125.3},
	{619.05, 105.9},
	{599.57, 87.44},
	{587.53, 75.47},
	{572.79, 60.2},
	{564.11, 50.9},
	{558.41, 44.63},
	{551.71, 37.14},
	};
	//void circfit(float * p, int n, int stride, float * out_a, float * out_b, float * out_r);
	float a;
	float b;
	float r;
	fitcirc((float*)&p, sizeof(p) / sizeof(vec2_t), 2, &a, &b, &r);
	printf("abr: %f %f %f\n", a, b, r);
	return;
}
