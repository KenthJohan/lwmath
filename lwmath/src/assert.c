#include "lwmath_assert.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lwmath_abort()
{
	printf("%s", "\n");
	fflush(stderr);
	fflush(stdout);
	abort();
}

int lwmath_assert_(
    const char *expr,
    const char *file,
    int32_t line,
    const char *fn,
    const char *fmt,
    ...)
{
	va_list args;
	va_start(args, fmt);
	int r = vprintf(fmt, args);
	va_end(args);
	lwmath_abort();
	return r;
}