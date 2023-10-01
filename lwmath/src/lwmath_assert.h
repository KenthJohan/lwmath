#pragma once
#include <stdint.h>


int lwmath_assert_(
	const char *expr, 
	const char *file, 
	int32_t line, 
	const char *fn, 
	const char *fmt, 
	...
	);


#define lwmath_assert(expr, ...) ((expr) ? 0: lwmath_assert_ (#expr, __FILE__, __LINE__, __func__, __VA_ARGS__))
#define lwmath_assert_notnull(expr) lwmath_assert(expr, "")