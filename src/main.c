#include <stdio.h>
#include "symtbl.h"

int main()
{
    printf("\n\n");

    FILE* asm_file = fopen("test.asm", "r");

    fill_symtbl(asm_file);

    return 0;
}
