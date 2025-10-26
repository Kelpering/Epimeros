#ifndef __EPIMEROS_INSTR_SET_RV32I_H__
#define __EPIMEROS_INSTR_SET_RV32I_H__
#include "parser.h"

extern instr_set RV32I;
extern reg_set RV32I_REGS;

instr_t* R_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
);
instr_t* I_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
);
instr_t* S_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
);
instr_t* B_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
);
instr_t* U_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
);
instr_t* J_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
);
instr_t* NO_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
);
instr_t* FENCE_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
);


uint32_t R_TRANS(
  instr_t* instr, 
  parser_ctx* ctx
);
uint32_t I_TRANS(
  instr_t* instr, 
  parser_ctx* ctx
);
uint32_t S_TRANS(
  instr_t* instr, 
  parser_ctx* ctx
);
uint32_t B_TRANS(
  instr_t* instr, 
  parser_ctx* ctx
);
uint32_t U_TRANS(
  instr_t* instr, 
  parser_ctx* ctx
);
uint32_t J_TRANS(
  instr_t* instr, 
  parser_ctx* ctx
);
uint32_t NO_TRANS(
  instr_t* instr, 
  parser_ctx* ctx
);
uint32_t FENCE_TRANS(
  instr_t* instr, 
  parser_ctx* ctx
);

#endif // __EPIMEROS_INSTR_SET_RV32I_H__