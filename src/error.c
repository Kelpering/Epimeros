#include "error.h"
#include <stdio.h>
#include <stdlib.h>

int line_num = 0;

void throw_error(const char* str)
{
    printf("Error (line %d): %s\n", line_num, str);

    exit(1);
}
