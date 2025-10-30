#include <stdlib.h>
#include <string.h>
#include "symtbl.h"
#include "error.h"

#include <stdio.h>

symbol_tbl* init_symtbl()
{
  symbol_tbl* symtbl = malloc(sizeof(symbol_tbl));

  symtbl->sym = malloc(sizeof(sym_t) * 16);
  symtbl->occupied = 0;
  symtbl->allocated = 16;

  for (int i = 0; i < 16; i++)
  {
    symtbl->sym[i].label = NULL;
    symtbl->sym[i].byte_offset = 0;
    symtbl->sym[i].type = SYM_EMPTY;
  }

  return symtbl;
}

void free_symtbl(symbol_tbl* symtbl)
{
  for (int i = 0; i < symtbl->occupied; i++)
    free(symtbl->sym[i].label);

  free(symtbl->sym);
  free(symtbl);

  return;
}

void expand_symtbl(symbol_tbl* symtbl)
{
  symtbl->allocated *= 2;
  sym_t* new = malloc(sizeof(sym_t) * (symtbl->allocated));
  for (int i = 0; i < symtbl->occupied; i++)
    new[i] = symtbl->sym[i];
  for (int i = symtbl->occupied; i < symtbl->allocated; i++)
  {
    new[i].byte_offset = 0;
    new[i].label = NULL;
    new[i].type = SYM_EMPTY;
  }

  free(symtbl->sym);
  symtbl->sym = new;
}

void allocate_symbol(const char* label, int byte_offset, symbol_tbl* symtbl)
{
  int sym_index;
  for (sym_index = 0; sym_index < symtbl->occupied; sym_index++)
  {
    if (strcmp(symtbl->sym[sym_index].label, label))
      continue;

    throw_error("Double label");
  }

  int str_size;
  for (str_size = 0; label[str_size]; str_size++)
    ;
  symtbl->sym[sym_index].label = malloc(str_size);

  for (int i = 0; i < str_size; i++)
    symtbl->sym[sym_index].label[i] = label[i];

  symtbl->sym[sym_index].byte_offset = byte_offset;
  symtbl->sym[sym_index].type = SYM_LABEL_ALLOCATED;

  symtbl->occupied++;
  if (symtbl->occupied >= symtbl->allocated)
    expand_symtbl(symtbl);

  return;
}

uint32_t search_symbol(const char* label, symbol_tbl* symtbl)
{
  uint32_t sym_index = 0;
  while (symtbl->sym[sym_index].type != SYM_EMPTY)
  {
    if (strcmp(symtbl->sym[sym_index].label, label))
    {
      sym_index++;
      continue;
    }

    return sym_index;
  }

  int str_size;
  for (str_size = 0; label[str_size]; str_size++)
    ;
  symtbl->sym[sym_index].label = malloc(str_size);

  for (int i = 0; i < str_size; i++)
    symtbl->sym[sym_index].label[i] = label[i];

  symtbl->sym[sym_index].byte_offset = 0;
  symtbl->sym[sym_index].type = SYM_LABEL_EXPECTED;

  symtbl->occupied++;
  if (symtbl->occupied >= symtbl->allocated)
    expand_symtbl(symtbl);

  return sym_index;
}

// We find a valid label
  // Look through the symtbl
  // Find an expected -> Overwrite allocated
  // Find an empty -> Overwrite allocated

// Parse Immediate (LABEL)
  // Look through symtbl
  // Find allocated -> Save index
  // Find empty (none found)
    // Save index
    // Overwrite expected

// During allocation / search
  // If we ever reach the end, double size of array and transfer

// During translation:
  // If we ever encounter an "expected", then we throw error "Missing Label"
