#ifndef __EPIMEROS_PRIVATE_PARSER_H__
#define __EPIMEROS_PRIVATE_PARSER_H__
#include "private/symtbl.h"
#include <stdint.h>
#include <stdio.h>

typedef struct parser_ctx parser_ctx;
typedef struct instr_def instr_def;
typedef struct reg_def reg_def;
typedef struct instr_t instr_t;
typedef struct instr_set instr_set;

struct parser_ctx
{
  uint64_t    offset;        // Current (and starting) offset in target memory
  symtbl_t*   symtbl;        // What symbols are defined (dynamic when parsing)
  instr_def** instr_def;     // What instructions are defined
  uint32_t    instr_count;   // Size of instr_def array
  reg_def**   reg_def;       // What registers are defined
  uint32_t    reg_count;     // Size of reg_def array
};

struct instr_def
{
  char* mnemonic;
  uint32_t opcode;
  int byte_width;
  int op_count;
  uint32_t (*trans_cb)(instr_t* instr, parser_ctx* ctx);
};

struct reg_def
{
  char* reg_str;
  uint32_t value;
};

struct instr_t
{
  struct instr_def* instr_def;
  struct instr_t* next;
  char* op[];
};

struct instr_set
{
  instr_def* instr_def_array;
  uint32_t   instr_def_size;
  reg_def*   reg_def_array;
  uint32_t   reg_def_size;
};

parser_ctx* init_parserctx(uint64_t start_addr, int set_count, ...);
void free_parserctx(parser_ctx* ctx);
instr_t* parse_file(FILE* file, parser_ctx* ctx);

#endif // __EPIMEROS_PRIVATE_PARSER_H__