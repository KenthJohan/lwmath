
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