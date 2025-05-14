#include "symtbl.h"
#include <stdio.h>
#include <stdlib.h>

int line_err = 1;
symbol symtbl[SYMTBL_SIZE] = {0};

static void clear_symtbl()
{
    // Free name pointers
    for (int i = 0; symtbl[i].type != NONE; i++)
    {
        free(symtbl[i].name);
        symtbl[i] = (symbol) {NULL, 0, 0};
    }

    return;    
}

static void normalize_line(char* line)
{

    int line_size = 0;
    while (line[line_size++] && line_size < MAX_LINE_SIZE);

    if (line_size >= MAX_LINE_SIZE)
    {
        printf("Error: Max line size exceeded. (Line: %d)\n", line_err);
        exit(1);
    }

    // Add newline if absent
    if (line[line_size-2] != '\n')
    {
        printf("Warn: Newline Absent (Line: %d)", line_err);
        line[line_size-2] = '\n';
    }

    for (int i = 0; i < MAX_LINE_SIZE; i++)
    {
        if (line[i] == '\0')
            break;

        // Decapitalize
        if (line[i] >= 'A' && line[i] <= 'Z')
        {
            line[i] += 32;
            continue;
        }

        // Remove line comments
        if (line[i] == '#')
        {
            // Line i+1 will not overflow due to previous check.
            line[i] = '\n';
            line[i+1] = '\0';
            break;
        }
    }

    return;
}

bool valid_char(char c)
{
    if (c >= 'A' && c <= 'Z')
        return true;

    if (c >= 'a' && c <= 'z')
        return true;

    if (c >= '0' && c <= '9')
        return true;

    if (c == '_')
        return true;

    return false;
}

void skip_until(char* line, char c, int* pos)
{
    while (line[*pos] != c && line[*pos] != '\n')
        (*pos)++;

    // Maybe return here for info on if we found char

    return;
}

static bool detect_label(char* line, int* pos, int* pc, char** symbol)
{
    while (line[*pos] == ' ')
        (*pos)++;

    char c = line[*pos];

    if (c == '\n')
        return false;

    // Directive
    if (c == '.')
    {
        //^ Handle directive action
        printf("TODO: Handle Directives in symtbl.c (Line %d)\n", line_err);
        exit(1);
    }

    if (!valid_char(c))
    {
        printf("Error: Unrecognized symbol \'%c\' (Line: %d)", c, line_err);
        exit(1);
    }

    int start_pos = *pos;
    skip_until(line, ' ', pos);
    (*symbol) = malloc(*pos - start_pos + 1);
    
    for (int i = 0; i < *pos - start_pos; i++)
    {
       (*symbol)[i] = line[start_pos + i];
    }
    (*symbol)[*pos - start_pos] = '\0';

    printf("symbol: %s\n", *symbol);


    return false;
}

void fill_symtbl(FILE* file)
{
    // Clear symtbl if needed
    if (symtbl[0].type != NONE)
        clear_symtbl();

    
    // Start from file beginning
    rewind(file);

    char line[MAX_LINE_SIZE+1];
    while (fgets(line, MAX_LINE_SIZE, file))
    {
        char* symbol;
        int pos = 0;
        int pc  = 0;
        normalize_line(line);
        
        while (detect_label(line, &pos, &pc, &symbol))
        {
            ;
        }
        
        
        line_err++;
    }
    

    return;
}
