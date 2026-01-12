#include "private/error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int line_num = 0;

void throw_error(const char* format, ...)
{
    va_list vargs;
    va_start(vargs, format);

    printf("Error: ");
    vprintf(format, vargs);
    printf("\n\n");

    exit(1);
}
