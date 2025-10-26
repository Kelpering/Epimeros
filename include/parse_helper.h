#ifndef __EPIMEROS_PARSE_HELPER_H__
#define __EPIMEROS_PARSE_HELPER_H__
#include "parser.h"

op_t parse_reg(const char* line, int* str_index, parser_ctx* ctx);
op_t parse_imm(const char* line, int* str_index, parser_ctx* ctx);






//? This file contains all of the helper functions, which are useful for
//? custom callback functions.



// CB funcs have to return an instr_t (see parser.h for details)

// next_op
  // Extract a token from start to ','
  //! Error: no more operands

// parse_reg
  // Parse registers

#endif // __EPIMEROS_PARSE_HELPER_H__
