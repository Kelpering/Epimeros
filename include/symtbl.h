#ifndef __EPIMEROS_SYMTBL_H__
#define __EPIMEROS_SYMTBL_H__
#include <stdint.h>

typedef enum sym_type
{
  SYM_EMPTY     = 0,
  SYM_LABEL_ALLOCATED = 1,
  SYM_LABEL_EXPECTED  = 2,
} sym_type;

typedef struct sym_t
{
  char* label;
  int byte_offset;
  sym_type type;
} sym_t;

typedef struct symtbl_t
{
  sym_t* sym;
  int occupied;
  int allocated;
} symbol_tbl;

symbol_tbl* init_symtbl();
void free_symtbl(symbol_tbl* symtbl);
void allocate_symbol(const char* label, int byte_offset, symbol_tbl* symtbl);
uint32_t search_symbol(const char* label, symbol_tbl* symtbl);

#endif // __EPIMEROS_SYMTBL_H__
