#ifndef __SYMTBL_H__
#define __SYMTBL_H__

#include <stdint.h>
#include <stdio.h>
#include "defines.h"

//- This will be used to generate and parse the symbol table.
//- Symbols will be labels or defined constants.

// The symbol type (affects relocation)
typedef enum 
{
    NONE = 0,   // An unset symbol
    LABEL,  // A relocatable label
    VALUE   // A constant
} symbol_type;

// A single valid symbol in the symtbl.
typedef struct
{
    char* name;
    uint64_t value;
    symbol_type type;
} symbol;

// A list of symbols. Each symbol name must be unique.
extern symbol symtbl[SYMTBL_SIZE];

// Fill static symtbl with symbols.
int fill_symtbl(FILE* file);

#endif // __SYMTBL_H__
