
#include "lwmath/lin.h"
#include "lwmath_assert.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

void vf32_sub(float * r, float const * a, float const * b, int n)
{
	lwmath_assert_notnull(r);
	lwmath_assert_notnull(a);
	lwmath_assert_notnull(b);
	for (int i = 0; i < n; i++)
	{
		r[i] = a[i] - b[i];
	}
}

void vf32_add_ab(float * r, float const * a, float const * b, int n, float a0, float b0)
{
	lwmath_assert_notnull(r);
	lwmath_assert_notnull(a);
	lwmath_assert_notnull(b);
	for (int i = 0; i < n; i++)
	{
		r[i] = a[i] * a0 + b[i] * b0;
	}
}


void find_minmax_f32(float *data, int n, float *out_min, float *out_max)
{
	float min = FLT_MAX;
	float max = FLT_MIN;
	for (int i = 0; i < n; ++i)
	{
		float a = data[i];
		if (a < min)
		{
			min = a;
		}
		if (a > max)
		{
			max = a;
		}
	}
	out_min[0] = min;
	out_max[0] = max;
}


void find_minmax_u32(uint32_t *data, int n, uint32_t *out_min, uint32_t *out_max)
{
	uint32_t min = UINT32_MAX;
	uint32_t max = 0;
	for (int i = 0; i < n; ++i)
	{
		uint32_t a = data[i];
		if (a < min)
		{
			min = a;
		}
		if (a > max)
		{
			max = a;
		}
	}
	out_min[0] = min;
	out_max[0] = max;
}

void find_minmax_u32_index(uint32_t *data, int n, int *out_min, int *out_max)
{
	uint32_t min = UINT32_MAX;
	uint32_t max = 0;
	int mini = 0;
	int maxi = 0;
	for (int i = 0; i < n; ++i)
	{
		uint32_t a = data[i];
		if (a < min)
		{
			min = a;
			mini = i;
		}
		if (a > max)
		{
			max = a;
			maxi = i;
		}
	}
	out_min[0] = mini;
	out_max[0] = maxi;
}


#define swap(t, a, b)   \
do {                 \
    t temp = a;    \
    a = b;           \
    b = temp;        \
} while(0)







void inverse2x2(mat2f_t * a, float determinant)
{
	//float determinant = MAT2F_DET(a);

	a->c0[1] = -a->c0[1];
	a->c1[0] = -a->c0[0];
	swap(float, a->c0[0], a->c0[1]);
	a->c0[0] /= determinant;
	a->c0[1] /= determinant;
	a->c1[0] /= determinant;
	a->c1[1] /= determinant;
}



void inverse3x3(mat3f_t * a, float determinant)
{

}









void circfit(float * p, int n, int stride, float * out_a, float * out_b, float * out_r)
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
	for (int i = 0; i < n; ++i, p += stride)
	{
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
	circfit((float*)&p, sizeof(p) / sizeof(vec2_t), 2, &a, &b, &r);
	printf("abr: %f %f %f\n", a, b, r);
	return;
}



uint32_t djb33_hash(const char* s, size_t len)
{
    uint32_t h = 5381;
    while (len--) {
        /* h = 33 * h ^ s[i]; */
        h += (h << 5);  
        h ^= *s++;
    }
    return h;
}