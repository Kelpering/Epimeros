#include <stdio.h>
#include "symtbl.h"
#include "parser.h"

int main()
{
    printf("\n\n");

    FILE* asm_file = fopen("test.asm", "r");

    int instr_cnt = fill_symtbl(asm_file);

    //- Print symbol table
    int i = 0;
    while (symtbl[i].type != NONE)
    {
        printf("%s (pc: %lu)\n", symtbl[i].name, symtbl[i].value);
        i++;
    }

    parse_file(asm_file, instr_cnt);



    return 0;
}
