#include "instr_set/RV32I.h"
#include "private/error.h"
#include "private/parser.h"
#include "private/str_parsing.h"

uint32_t r_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 7;
  bytecode |= parse_reg(instr->op[1], ctx) << 15;
  bytecode |= parse_reg(instr->op[2], ctx) << 20;

  return bytecode;
}

uint32_t i_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 7;
  bytecode |= parse_reg(instr->op[1], ctx) << 15;

  int64_t val = parse_imm(instr->op[2], ctx);
  uint32_t imm = process_bit_range(val, 12, true);

  bytecode |= (((imm & 0b000000000001) >> 0) << 20);
  bytecode |= (((imm & 0b000000011110) >> 1) << 21);
  bytecode |= (((imm & 0b111111100000) >> 5) << 25);

  return bytecode;
}

uint32_t s_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 15;
  bytecode |= parse_reg(instr->op[1], ctx) << 20;

  int64_t val = parse_imm(instr->op[2], ctx);
  uint32_t imm = process_bit_range(val, 12, true);

  bytecode |= (((imm & 0b000000000001) >> 0) << 7);
  bytecode |= (((imm & 0b000000011110) >> 1) << 8);
  bytecode |= (((imm & 0b111111100000) >> 5) << 25);

  return bytecode;
}

uint32_t b_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 15;
  bytecode |= parse_reg(instr->op[1], ctx) << 20;

  int64_t val = parse_imm(instr->op[2], ctx);
  uint32_t imm = process_bit_range(val, 13, true);

  if (imm % 2)
    throw_error("Immediate Out of Alignment (Must be multiple of two)");

  bytecode |= (((imm & 0b0000000011110) >>  1) <<  8);
  bytecode |= (((imm & 0b0011111100000) >>  5) << 25);
  bytecode |= (((imm & 0b0100000000000) >> 11) <<  7);
  bytecode |= (((imm & 0b1000000000000) >> 12) << 31);

  return bytecode;
}

uint32_t u_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 7;

  int64_t val = parse_imm(instr->op[1], ctx);
  uint32_t imm = process_bit_range(val, 20, true);

  bytecode |= (((imm & 0b00000000000011111111) >>  0) << 12);
  bytecode |= (((imm & 0b11111111111100000000) >>  8) << 20);

  return bytecode;
}

uint32_t j_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 7;

  int64_t val = parse_imm(instr->op[1], ctx);
  uint32_t imm = process_bit_range(val, 21, true);

  if (imm % 2)
    throw_error("Immediate Out of Alignment (Must be multiple of two)");

  bytecode |= (((imm & 0b000000000000000011110) >>  1) << 21);
  bytecode |= (((imm & 0b000000000011111100000) >>  5) << 25);
  bytecode |= (((imm & 0b000000000100000000000) >> 11) << 20);
  bytecode |= (((imm & 0b011111111000000000000) >> 12) << 12);
  bytecode |= (((imm & 0b100000000000000000000) >> 20) << 31);

  return bytecode;
}

uint32_t fence_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;

  uint8_t flag_pred = 0;
  for (int i = 0; instr->op[0][i]; i++)
  {
    switch (instr->op[0][i]) 
    {
      case 'I':
      case 'i':
        if (flag_pred & 0b1000)
          throw_error("Duplicate fence flag");
        flag_pred |= 0b1000;
        break;

      case 'O':
      case 'o':
        if (flag_pred & 0b0100)
          throw_error("Duplicate fence flag");
        flag_pred |= 0b0100;
        break;
      
      case 'R':
      case 'r':
        if (flag_pred & 0b0010)
          throw_error("Duplicate fence flag");
        flag_pred |= 0b0010;
        break;

      case 'W':
      case 'w':
        if (flag_pred & 0b0001)
          throw_error("Duplicate fence flag");
        flag_pred |= 0b0001;
        break;

      default:
        throw_error("Unknown flag in fence");
    }
  }

  uint8_t flag_succ = 0;
  for (int i = 0; instr->op[1][i]; i++)
  {
    switch (instr->op[1][i]) 
    {
      case 'I':
      case 'i':
        if (flag_succ & 0b1000)
          throw_error("Duplicate fence flag");
        flag_succ |= 0b1000;
        break;

      case 'O':
      case 'o':
        if (flag_succ & 0b0100)
          throw_error("Duplicate fence flag");
        flag_succ |= 0b0100;
        break;
      
      case 'R':
      case 'r':
        if (flag_succ & 0b0010)
          throw_error("Duplicate fence flag");
        flag_succ |= 0b0010;
        break;

      case 'W':
      case 'w':
        if (flag_succ & 0b0001)
          throw_error("Duplicate fence flag");
        flag_succ |= 0b0001;
        break;

      default:
        throw_error("Unknown flag in fence");
    }
  }

  bytecode |= flag_succ << 20;
  bytecode |= flag_pred << 24;

  return bytecode;
}

uint32_t no_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  return bytecode;
}

static instr_def instr_array[] = 
{
  {"lui",    0b00000000000000000000000000110111, 4, 2,     u_trans}, 
  {"auipc",  0b00000000000000000000000000010111, 4, 2,     u_trans},
  {"jal",    0b00000000000000000000000001101111, 4, 2,     j_trans},
  {"jalr",   0b00000000000000000000000001100111, 4, 3,     i_trans},
  {"beq",    0b00000000000000000000000001100011, 4, 3,     b_trans},
  {"bne",    0b00000000000000000001000001100011, 4, 3,     b_trans},
  {"blt",    0b00000000000000000100000001100011, 4, 3,     b_trans},
  {"bge",    0b00000000000000000101000001100011, 4, 3,     b_trans},
  {"bltu",   0b00000000000000000110000001100011, 4, 3,     b_trans},
  {"bgeu",   0b00000000000000000111000001100011, 4, 3,     b_trans},
  {"lb",     0b00000000000000000000000000000011, 4, 3,     i_trans},
  {"lh",     0b00000000000000000001000000000011, 4, 3,     i_trans},
  {"lw",     0b00000000000000000010000000000011, 4, 3,     i_trans},
  {"lbu",    0b00000000000000000100000000000011, 4, 3,     i_trans},
  {"lhu",    0b00000000000000000101000000000011, 4, 3,     i_trans},
  {"sb",     0b00000000000000000000000000100011, 4, 3,     s_trans},
  {"sh",     0b00000000000000000001000000100011, 4, 3,     s_trans},
  {"sw",     0b00000000000000000010000000100011, 4, 3,     s_trans},
  {"addi",   0b00000000000000000000000000010011, 4, 3,     i_trans},
  {"slti",   0b00000000000000000010000000010011, 4, 3,     i_trans},
  {"sltiu",  0b00000000000000000011000000010011, 4, 3,     i_trans},
  {"xori",   0b00000000000000000100000000010011, 4, 3,     i_trans},
  {"ori",    0b00000000000000000110000000010011, 4, 3,     i_trans},
  {"andi",   0b00000000000000000111000000010011, 4, 3,     i_trans},
  {"slli",   0b00000000000000000001000000010011, 4, 3,     r_trans},
  {"srli",   0b00000000000000000101000000010011, 4, 3,     r_trans},
  {"srai",   0b01000000000000000101000000010011, 4, 3,     r_trans},
  {"add",    0b00000000000000000000000000110011, 4, 3,     r_trans},
  {"sub",    0b01000000000000000000000000110011, 4, 3,     r_trans},
  {"sll",    0b00000000000000000001000000110011, 4, 3,     r_trans},
  {"slt",    0b00000000000000000010000000110011, 4, 3,     r_trans},
  {"sltu",   0b00000000000000000011000000110011, 4, 3,     r_trans},
  {"xor",    0b00000000000000000100000000110011, 4, 3,     r_trans},
  {"srl",    0b00000000000000000101000000110011, 4, 3,     r_trans},
  {"sra",    0b01000000000000000101000000110011, 4, 3,     r_trans},
  {"or",     0b00000000000000000110000000110011, 4, 3,     r_trans},
  {"and",    0b00000000000000000111000000110011, 4, 3,     r_trans},
  {"fence",  0b00000000000000000000000000001111, 4, 2, fence_trans}, 
  {"ecall",  0b00000000000000000000000001110011, 4, 0,    no_trans},
  {"ebreak", 0b00000000000100000000000001110011, 4, 0,    no_trans},
};

static reg_def reg_array[] = 
{
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
};

instr_set RV32I = 
{
  instr_array,
  sizeof(instr_array)/sizeof(instr_def),
  reg_array,
  sizeof(reg_array)/sizeof(reg_def)
};