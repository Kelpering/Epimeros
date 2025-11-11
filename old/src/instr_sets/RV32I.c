#include "instr_sets/RV32I.h"
#include "error.h"
#include "parse_helper.h"
#include <stdlib.h>

#include <stdio.h>

instr_set RV32I = 
{
  {"lui",    0b00000000000000000000000000110111, 4, U_PARSE,     U_TRANS},
  {"auipc",  0b00000000000000000000000000010111, 4, U_PARSE,     U_TRANS},
  {"jal",    0b00000000000000000000000001101111, 4, J_PARSE,     J_TRANS},
  {"jalr",   0b00000000000000000000000001100111, 4, I_PARSE,     I_TRANS},
  {"beq",    0b00000000000000000000000001100011, 4, B_PARSE,     B_TRANS},
  {"bne",    0b00000000000000000001000001100011, 4, B_PARSE,     B_TRANS},
  {"blt",    0b00000000000000000100000001100011, 4, B_PARSE,     B_TRANS},
  {"bge",    0b00000000000000000101000001100011, 4, B_PARSE,     B_TRANS},
  {"bltu",   0b00000000000000000110000001100011, 4, B_PARSE,     B_TRANS},
  {"bgeu",   0b00000000000000000111000001100011, 4, B_PARSE,     B_TRANS},
  {"lb",     0b00000000000000000000000000000011, 4, I_PARSE,     I_TRANS},
  {"lh",     0b00000000000000000001000000000011, 4, I_PARSE,     I_TRANS},
  {"lw",     0b00000000000000000010000000000011, 4, I_PARSE,     I_TRANS},
  {"lbu",    0b00000000000000000100000000000011, 4, I_PARSE,     I_TRANS},
  {"lhu",    0b00000000000000000101000000000011, 4, I_PARSE,     I_TRANS},
  {"sb",     0b00000000000000000000000000100011, 4, S_PARSE,     S_TRANS},
  {"sh",     0b00000000000000000001000000100011, 4, S_PARSE,     S_TRANS},
  {"sw",     0b00000000000000000010000000100011, 4, S_PARSE,     S_TRANS},
  {"addi",   0b00000000000000000000000000010011, 4, I_PARSE,     I_TRANS},
  {"slti",   0b00000000000000000010000000010011, 4, I_PARSE,     I_TRANS},
  {"sltiu",  0b00000000000000000011000000010011, 4, I_PARSE,     I_TRANS},
  {"xori",   0b00000000000000000100000000010011, 4, I_PARSE,     I_TRANS},
  {"ori",    0b00000000000000000110000000010011, 4, I_PARSE,     I_TRANS},
  {"andi",   0b00000000000000000111000000010011, 4, I_PARSE,     I_TRANS},
  {"slli",   0b00000000000000000001000000010011, 4, R_PARSE,     R_TRANS},
  {"srli",   0b00000000000000000101000000010011, 4, R_PARSE,     R_TRANS},
  {"srai",   0b01000000000000000101000000010011, 4, R_PARSE,     R_TRANS},
  {"add",    0b00000000000000000000000000110011, 4, R_PARSE,     R_TRANS},
  {"sub",    0b01000000000000000000000000110011, 4, R_PARSE,     R_TRANS},
  {"sll",    0b00000000000000000001000000110011, 4, R_PARSE,     R_TRANS},
  {"slt",    0b00000000000000000010000000110011, 4, R_PARSE,     R_TRANS},
  {"sltu",   0b00000000000000000011000000110011, 4, R_PARSE,     R_TRANS},
  {"xor",    0b00000000000000000100000000110011, 4, R_PARSE,     R_TRANS},
  {"srl",    0b00000000000000000101000000110011, 4, R_PARSE,     R_TRANS},
  {"sra",    0b01000000000000000101000000110011, 4, R_PARSE,     R_TRANS},
  {"or",     0b00000000000000000110000000110011, 4, R_PARSE,     R_TRANS},
  {"and",    0b00000000000000000111000000110011, 4, R_PARSE,     R_TRANS},
  {"fence",  0b00000000000000000000000000001111, 4, FENCE_PARSE, FENCE_TRANS}, 
  {"ecall",  0b00000000000000000000000001110011, 4, NO_PARSE,    NO_TRANS},
  {"ebreak", 0b00000000000100000000000001110011, 4, NO_PARSE,    NO_TRANS},
  NULL
};

reg_set RV32I_REGS = 
{
  // Default
  {"x0",    0},
  {"x1",    1},
  {"x2",    2},
  {"x3",    3},
  {"x4",    4},
  {"x5",    5},
  {"x6",    6},
  {"x7",    7},
  {"x8",    8},
  {"x9",    9},
  {"x10",  10},
  {"x11",  11},
  {"x12",  12},
  {"x13",  13},
  {"x14",  14},
  {"x15",  15},
  {"x16",  16},
  {"x17",  17},
  {"x18",  18},
  {"x19",  19},
  {"x20",  20},
  {"x21",  21},
  {"x22",  22},
  {"x23",  23},
  {"x24",  24},
  {"x25",  25},
  {"x26",  26},
  {"x27",  27},
  {"x28",  28},
  {"x29",  29},
  {"x30",  30},
  {"x31",  31},

  // ABI
  {"zero", 0},
  {"ra",   1},
  {"sp",   2},
  {"gp",   3},
  {"tp",   4},
  {"t0",   5},
  {"t1",   6},
  {"t2",   7},
  {"s0",   8}, 
  {"fp",   8},
  {"s1",   9},
  {"a0",  10},
  {"a1",  11},
  {"a2",  12},
  {"a3",  13},
  {"a4",  14},
  {"a5",  15},
  {"a6",  16},
  {"a7",  17},
  {"s2",  18},
  {"s3",  19},
  {"s4",  20},
  {"s5",  21},
  {"s6",  22},
  {"s7",  23},
  {"s8",  24},
  {"s9",  25},
  {"s10", 26},
  {"s11", 27},
  {"t3",  28},
  {"t4",  29},
  {"t5",  30},
  {"t6",  31},
  NULL
};

instr_t* R_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
)
{
  instr_t* instr = malloc(sizeof(instr_t));
  instr->def = instr_def;
  instr->op_size = 3;
  instr->op = malloc(sizeof(op_t) * instr->op_size);
  instr->next = NULL;
  
  int str_index = 0;
  instr->op[0] = parse_reg(line, &str_index, ctx);
  instr->op[1] = parse_reg(line, &str_index, ctx);
  instr->op[2] = parse_reg(line, &str_index, ctx);
  if (line[str_index] != '\0')
    throw_error("Excess operand");

  return instr;
}

instr_t* I_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
)
{
  instr_t* instr = malloc(sizeof(instr_t));
  instr->def = instr_def;
  instr->op_size = 3;
  instr->op = malloc(sizeof(op_t) * instr->op_size);
  instr->next = NULL;
  
  int str_index = 0;
  instr->op[0] = parse_reg(line, &str_index, ctx);
  instr->op[1] = parse_reg(line, &str_index, ctx);
  instr->op[2] = parse_imm(line, &str_index, ctx);
  if (line[str_index] != '\0')
    throw_error("Excess operand");

  return instr;
}

instr_t* S_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
)
{
  instr_t* instr = malloc(sizeof(instr_t));
  instr->def = instr_def;
  instr->op_size = 3;
  instr->op = malloc(sizeof(op_t) * instr->op_size);
  instr->next = NULL;
  
  int str_index = 0;
  instr->op[0] = parse_reg(line, &str_index, ctx);
  instr->op[1] = parse_reg(line, &str_index, ctx);
  instr->op[2] = parse_imm(line, &str_index, ctx);
  if (line[str_index] != '\0')
    throw_error("Excess operand");

  return instr;
}

instr_t* B_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
)
{
  instr_t* instr = malloc(sizeof(instr_t));
  instr->def = instr_def;
  instr->op_size = 3;
  instr->op = malloc(sizeof(op_t) * instr->op_size);
  instr->next = NULL;
  
  int str_index = 0;
  instr->op[0] = parse_reg(line, &str_index, ctx);
  instr->op[1] = parse_reg(line, &str_index, ctx);
  instr->op[2] = parse_imm(line, &str_index, ctx);
  if (line[str_index] != '\0')
    throw_error("Excess operand");

  return instr;
}

instr_t* U_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
)
{
  instr_t* instr = malloc(sizeof(instr_t));
  instr->def = instr_def;
  instr->op_size = 2;
  instr->op = malloc(sizeof(op_t) * instr->op_size);
  instr->next = NULL;
  
  int str_index = 0;
  instr->op[0] = parse_reg(line, &str_index, ctx);
  instr->op[1] = parse_imm(line, &str_index, ctx);
  if (line[str_index] != '\0')
    throw_error("Excess operand");

  return instr;
}

instr_t* J_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
)
{
  instr_t* instr = malloc(sizeof(instr_t));
  instr->def = instr_def;
  instr->op_size = 2;
  instr->op = malloc(sizeof(op_t) * instr->op_size);
  instr->next = NULL;
  
  int str_index = 0;
  instr->op[0] = parse_reg(line, &str_index, ctx);
  instr->op[1] = parse_imm(line, &str_index, ctx);
  if (line[str_index] != '\0')
    throw_error("Excess operand");

  return instr;
}

instr_t* NO_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
)
{
  instr_t* instr = malloc(sizeof(instr_t));
  instr->def = instr_def;
  instr->op_size = 0;
  instr->op = malloc(sizeof(op_t) * instr->op_size);
  instr->next = NULL;

  if (line[0] != '\0')
    throw_error("Excess operand");

  return instr;
}

instr_t* FENCE_PARSE(
  const char* line, 
  const instr_def_t* instr_def, 
  parser_ctx* ctx
)
{
  //! Must be custom
}

uint32_t R_TRANS(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->def->opcode;
  bytecode |= instr->op[0].val.u64 << 7;
  bytecode |= instr->op[1].val.u64 << 15;
  bytecode |= instr->op[2].val.u64 << 20;

  return bytecode;
}

uint32_t I_TRANS(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->def->opcode;
  bytecode |= instr->op[0].val.u64 << 7;
  bytecode |= instr->op[1].val.u64 << 15;

  int64_t imm = instr->op[2].val.i64;
  if (instr->op[2].type == MACRO)
    imm = instr->op[2].macro_cb(imm, ctx);

  if ((imm > (1<<12)-1) | (imm < -(1<<11)))
    throw_error("Immediate Out of Range");

  bytecode |= (((imm & 0b00000000001) >> 0) << 20);
  bytecode |= (((imm & 0b00000011110) >> 1) << 21);
  bytecode |= (((imm & 0b11111100000) >> 5) << 25);
  bytecode |= (imm < 0) ? 1 << 31 : 0;

  return bytecode;
}

uint32_t S_TRANS(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->def->opcode;
  bytecode |= instr->op[0].val.u64 << 15;
  bytecode |= instr->op[1].val.u64 << 20;

  int64_t imm = instr->op[2].val.i64;
  if (instr->op[2].type == MACRO)
    imm = instr->op[2].macro_cb(imm, ctx);

  if ((imm > (1<<11)-1) | (imm < -(1<<11)))
    throw_error("Immediate Out of Range");

  bytecode |= (((imm & 0b00000000001) >> 0) << 7);
  bytecode |= (((imm & 0b00000011110) >> 1) << 8);
  bytecode |= (((imm & 0b11111100000) >> 5) << 25);
  bytecode |= (imm < 0) ? 1 << 31 : 0;

  return bytecode;
}

uint32_t B_TRANS(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->def->opcode;
  bytecode |= instr->op[0].val.u64 << 15;
  bytecode |= instr->op[1].val.u64 << 20;

  int64_t imm = instr->op[2].val.i64;
  if (instr->op[2].type == MACRO)
    imm = instr->op[2].macro_cb(imm, ctx);

  if (imm % 2)
    throw_error("Immediate Out of Alignment");
  if ((imm > (1<<12)-1) | (imm < -(1<<12)))
    throw_error("Immediate Out Of Range");

  bytecode |= (((imm & 0b000000011110) >>  1) <<  8);
  bytecode |= (((imm & 0b011111100000) >>  5) << 25);
  bytecode |= (((imm & 0b100000000000) >> 11) <<  7);
  bytecode |= (imm < 0) ? 1 << 31 : 0;

  return bytecode;
}

uint32_t U_TRANS(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->def->opcode;
  bytecode |= instr->op[0].val.u64 << 7;

  int64_t imm = instr->op[1].val.i64;
  if (instr->op[1].type == MACRO)
    imm = instr->op[1].macro_cb(imm, ctx);

  if ((imm > (1L<<20)-1) | (imm < -(1L<<19)))
    throw_error("Immediate Out Of Range");

  bytecode |= (((imm & 0b0000000000011111111) >>  0) << 12);
  bytecode |= (((imm & 0b1111111111100000000) >>  8) << 20);
  bytecode |= (imm < 0) ? 1 << 31 : 0;

  return bytecode;
}

uint32_t J_TRANS(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->def->opcode;
  bytecode |= instr->op[0].val.u64 << 7;

  int64_t imm = instr->op[1].val.i64;
  if (instr->op[1].type == MACRO)
    imm = instr->op[1].macro_cb(imm, ctx);

  if (imm % (1<<1))
    throw_error("Immediate Out of Alignment");
  if ((imm > (1L<<20)-1) | (imm < -(1L<<20)))
    throw_error("Immediate Out Of Range");

  bytecode |= (((imm & 0b00000000000000011110) >>  1) << 21);
  bytecode |= (((imm & 0b00000000011111100000) >>  5) << 25);
  bytecode |= (((imm & 0b00000000100000000000) >> 11) << 20);
  bytecode |= (((imm & 0b11111111000000000000) >> 12) << 12);
  bytecode |= (imm < 0) ? 1 << 31 : 0;

  return bytecode;
}

uint32_t NO_TRANS(instr_t* instr, parser_ctx* ctx)
{
  return instr->def->opcode;
}

uint32_t FENCE_TRANS(instr_t* instr, parser_ctx* ctx)
{

}