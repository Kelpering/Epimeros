#include <stdio.h>
#include "parser.h"
#include "translator.h"
#include <stdlib.h>

int main()
{
    const char* code = 
    "addi x1, x2, 100\n"
    "\n"
    "sub  x1, x2, x3\n"
    ;

    // Accept any string fragment with a letter as the first non-whitespace char
    // Reject any string fragment with only whitespace
    // Split by \n

    int str_len = 0;
    while (code[str_len++]);
    char* str_cpy = malloc(str_len);

    //? This translates the singular string into multiple null-terminated lines.
    //? This could be seen as reading a file line-by-line.
    for (int i = 0; i < str_len; i++)
    {
        str_cpy[i] = code[i];
        if (code[i] == '\n')
            str_cpy[i] = '\0';
    }
    
    for (int str_index = 0; str_index < str_len; )
    {
        instr_t instr = parse_instruction(&str_cpy[str_index]);
        if (instr.mnemonic == ERROR_MNEMONIC)
            goto next_line;   // Skip empty lines

        uint32_t hex = translate_instr(instr);

        printf("Hex: 0x%x\n", hex);

    next_line:
        while (str_cpy[str_index++]);
    }
    free(str_cpy);

    return 0;
}
