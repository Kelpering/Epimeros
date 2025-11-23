#include "private/symtbl.h"
#include "private/error.h"
#include <alloca.h>
#include <stdlib.h>
#include <string.h>

void expand_symtbl(uint32_t new_capacity, symtbl_t* symtbl);

symtbl_t* init_symtbl()
{
  symtbl_t* symtbl = malloc(sizeof(symtbl_t));
  if(!symtbl)
    throw_error("Out of Memory");

  symtbl->sym = NULL;
  symtbl->size = 0;
  symtbl->capacity = 0;
  expand_symtbl(16, symtbl);

  return symtbl;
}

void free_symtbl(symtbl_t* symtbl)
{
  for (int i = 0; i < symtbl->capacity; i++)
    free(symtbl->sym[i].symbol);
  free(symtbl->sym);
  free(symtbl);

  return;
}

void expand_symtbl(uint32_t new_capacity, symtbl_t* symtbl)
{
  if (symtbl->capacity >= new_capacity)
    throw_error("Cannot shrink SymTbl capacity");

  symtbl->capacity = new_capacity;
  symbol_t* new = malloc(sizeof(symbol_t) * (symtbl->capacity));
  if(!new)
    throw_error("Out of Memory");

  int i;
  for (i = 0; i < symtbl->size; i++)
    new[i] = symtbl->sym[i];
  for (; i < symtbl->capacity; i++)
  {
    new[i].symbol = NULL;
    new[i].type = SYM_EMPTY;
    new[i].byte_offset = 0;
  }

  free(symtbl->sym);
  symtbl->sym = new;

  return;
}

symbol_t* alloc_symbol(const char* symbol, uint64_t val, symtbl_t* symtbl)
{
  symbol_t* sym = search_symtbl(symbol, symtbl);
  if (sym->type == SYM_ALLOCATED)
    throw_error("Double symbol \"%s\"", sym->symbol);

  // char* symbol already set by search_symtbl
  sym->type = SYM_ALLOCATED;
  sym->byte_offset = val;

  // Never allow symtbl to become full
  symtbl->size++;
  if (symtbl->capacity <= symtbl->size)
    expand_symtbl(symtbl->capacity * 2, symtbl);

  return sym;
}

symbol_t* search_symtbl(const char* symbol, symtbl_t* symtbl)
{
  // Never fails since symtbl is always over-allocated.
  uint32_t sym_index;
  for (sym_index = 0; symtbl->sym[sym_index].type != SYM_EMPTY; sym_index++)
  {
    if (strcmp(symbol, symtbl->sym[sym_index].symbol) == 0)
      return &symtbl->sym[sym_index];
  }

  //? Expect a forward reference (future-defined) symbol.
  symbol_t* sym = &symtbl->sym[sym_index];
  sym->symbol = strdup(symbol);
  sym->type = SYM_EXPECTED;
  sym->byte_offset = 0;   // Value must later be resolved before translation

  // Never allow symtbl to become full
  symtbl->size++;
  if (symtbl->capacity <= symtbl->size)
    expand_symtbl(symtbl->capacity * 2, symtbl);

  return sym;
}

symbol_t* search_symtbl_strict(const char* symbol, symtbl_t* symtbl)
{
  symbol_t* sym = search_symtbl(symbol, symtbl);
  if (sym->type != SYM_ALLOCATED)
    throw_error("Missing symbol");
  return sym;
}