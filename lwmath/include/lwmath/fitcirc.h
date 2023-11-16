#pragma once

void fitcirc(void * data, int n, int stride, float * out_a, float * out_b, float * out_r);

typedef enum 
{
	FITCIRC_L1,
	FITCIRC_L2,
	FITCIRC_CMP,
	FITCIRC_ABS,
} fitcirc_l_t;
typedef enum 
{
	FITCIRC_EFN_EQM,
	FITCIRC_EFN_SIGNED,
	FITCIRC_EFN_OUTLIERS,
	FITCIRC_EFN_ABS,
} fitcirc_efn_t;

float fitcirc_error1(float x, float y, float a, float b, float r, fitcirc_l_t l);
float fitcirc_error(void *data, int n, int stride, float a, float b, float r, fitcirc_efn_t efx);