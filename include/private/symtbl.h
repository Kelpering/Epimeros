#ifndef __EPIMEROS_PRIVATE_SYMTBL_H__
#define __EPIMEROS_PRIVATE_SYMTBL_H__
#include <stdint.h>

typedef enum sym_type
{
  SYM_EMPTY     = 0,
  SYM_ALLOCATED = 1,
  SYM_EXPECTED  = 2,
} sym_type;

typedef struct symbol_t
{
  char* symbol;
  sym_type type;
  uint64_t byte_offset; 
} symbol_t;

// Symtbl is an amortized dynamic array
typedef struct symtbl_t
{
  symbol_t* sym;
  uint32_t size;
  uint32_t capacity;
} symtbl_t;

symtbl_t* init_symtbl();
void free_symtbl(symtbl_t* symtbl);
symbol_t* alloc_symbol(const char* symbol, uint64_t val, symtbl_t* symtbl);
symbol_t* search_symtbl(const char* symbol, symtbl_t* symtbl);
symbol_t* search_symtbl_strict(const char* symbol, symtbl_t* symtbl);

#endif // __EPIMEROS_PRIVATE_SYMTBL_H__