#pragma once

void fitcirc(void * data, int n, int stride, float * out_a, float * out_b, float * out_r);
float fitcirc_error(float x, float y, float a, float b, float r);
float fitcirc_error2(void * data, int n, int stride, float a, float b, float r);