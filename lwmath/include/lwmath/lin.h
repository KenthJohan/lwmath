#pragma once
#include <stdint.h>
#include <stdlib.h>


typedef struct
{
	float c0[2];
	float c1[2];
} mat2f_t;
#define MAT2F_DET(x) (1.0f/((x)->c0[0] * (x)->c1[1]) - ((x)->c0[1] * (x)->c1[0]))


typedef struct
{
	float c0[3];
	float c1[3];
	float c2[3];
} mat3f_t;



typedef struct
{
	float x;
	float y;
} vec2_t;

typedef struct
{
	float x;
	float y;
	float z;
} vec3_t;




void inverse2x2(mat2f_t * a, float determinant);


void vf32_sub(float * r, float const * a, float const * b, int n);

void vf32_add_ab(float * r, float const * a, float const * b, int n, float a0, float b0);

void find_minmax_f32(float *data, int n, float *out_min, float *out_max);

void find_minmax_u32(uint32_t *data, int n, uint32_t *out_min, uint32_t *out_max);

void find_minmax_u32_index(uint32_t *data, int n, int *out_min, int *out_max);


void circfit(float * p, int n, int stride, float * out_a, float * out_b, float * out_r);


uint32_t djb33_hash(const char* s, size_t len);