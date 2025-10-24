#ifndef __EPIMEROS_PARSE_HELPER_H__
#define __EPIMEROS_PARSE_HELPER_H__
#include "symtbl.h"

//? This file contains all of the helper functions, which are useful for
//? custom callback functions.

struct parser_ctx;

typedef struct instr_def_t
{ 
  char* mnemonic;
  uint32_t opcode;
  int byte_size;
  instr_t* (*parse_cb)(const char* line, instr_def_t* def, parser_ctx* ctx);
  void (*trans_cb)(instr_t* instr, parser_ctx* ctx);

} instr_def_t;
typedef const instr_def_t instr_set[];

typedef struct reg_def_t
{
  char* str;
  uint8_t reg_index;
} reg_def_t;
typedef const reg_def_t reg_set[];

typedef struct parser_ctx
{
  instr_def_t* instr_list;
  int instr_count;
  reg_def_t* reg_list;
  int reg_count;
  symbol_tbl* symtbl;
} parser_ctx;

void  R_PARSE(const char* line, instr_def_t* instr_def);
void  I_PARSE(const char* line, instr_def_t* instr_def);
void  S_PARSE(const char* line, instr_def_t* instr_def);
void  B_PARSE(const char* line, instr_def_t* instr_def);
void  U_PARSE(const char* line, instr_def_t* instr_def);
void  J_PARSE(const char* line, instr_def_t* instr_def);
void NO_PARSE(const char* line, instr_def_t* instr_def);

// CB funcs have to return an instr_t (see parser.h for details)

// next_op
  // Extract a token from start to ','
  //! Error: no more operands

// parse_reg
  // Parse registers

parser_ctx* init_parserctx(instr_set* sets[],  reg_set* reg_sets[]);
void free_parserctx(parser_ctx* ctx);

#endif // __EPIMEROS_PARSE_HELPER_H__