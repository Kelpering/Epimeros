#include "instr_set/RV64I.h"
#include "private/error.h"
#include "private/parser.h"
#include "private/str_parsing.h"

static uint32_t r_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 7;
  bytecode |= parse_reg(instr->op[1], ctx) << 15;
  bytecode |= parse_reg(instr->op[2], ctx) << 20;

  return bytecode;
}

static uint32_t i_trans(instr_t* instr, parser_ctx* ctx)
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

static uint32_t shamt_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 7;
  bytecode |= parse_reg(instr->op[1], ctx) << 15;

  int64_t val = parse_imm(instr->op[2], ctx);
  uint32_t shamt = process_bit_range(val, 6, false);
  bytecode |= (shamt << 20);

  return bytecode;
}

static uint32_t shamtw_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 7;
  bytecode |= parse_reg(instr->op[1], ctx) << 15;

  int64_t val = parse_imm(instr->op[2], ctx);
  uint32_t shamt = process_bit_range(val, 5, false);
  bytecode |= (shamt << 20);

  return bytecode;
}

static uint32_t s_trans(instr_t* instr, parser_ctx* ctx)
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

static uint32_t b_trans(instr_t* instr, parser_ctx* ctx)
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

static uint32_t u_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  bytecode |= parse_reg(instr->op[0], ctx) << 7;

  int64_t val = parse_imm(instr->op[1], ctx);
  uint32_t imm = process_bit_range(val, 20, true);

  bytecode |= (((imm & 0b00000000000011111111) >>  0) << 12);
  bytecode |= (((imm & 0b11111111111100000000) >>  8) << 20);

  return bytecode;
}

static uint32_t j_trans(instr_t* instr, parser_ctx* ctx)
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

static uint32_t fence_trans(instr_t* instr, parser_ctx* ctx)
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

static uint32_t no_trans(instr_t* instr, parser_ctx* ctx)
{
  uint32_t bytecode = instr->instr_def->opcode;
  return bytecode;
}

static instr_def instr_array[] = 
{
  {"lui",       0x00000037, 4, 2,      u_trans}, 
  {"auipc",     0x00000017, 4, 2,      u_trans},
  {"jal",       0x0000006F, 4, 2,      j_trans},
  {"jalr",      0x00000067, 4, 3,      i_trans},
  {"beq",       0x00000063, 4, 3,      b_trans},
  {"bne",       0x00001063, 4, 3,      b_trans},
  {"blt",       0x00004063, 4, 3,      b_trans},
  {"bge",       0x00005063, 4, 3,      b_trans},
  {"bltu",      0x00006063, 4, 3,      b_trans},
  {"bgeu",      0x00007063, 4, 3,      b_trans},
  {"lb",        0x00000003, 4, 3,      i_trans},
  {"lh",        0x00001003, 4, 3,      i_trans},
  {"lw",        0x00002003, 4, 3,      i_trans},
  {"lbu",       0x00004003, 4, 3,      i_trans},
  {"lhu",       0x00005003, 4, 3,      i_trans},
  {"sb",        0x00000023, 4, 3,      s_trans},
  {"sh",        0x00001023, 4, 3,      s_trans},
  {"sw",        0x00002023, 4, 3,      s_trans},
  {"addi",      0x00000013, 4, 3,      i_trans},
  {"slti",      0x00002013, 4, 3,      i_trans},
  {"sltiu",     0x00003013, 4, 3,      i_trans},
  {"xori",      0x00004013, 4, 3,      i_trans},
  {"ori",       0x00006013, 4, 3,      i_trans},
  {"andi",      0x00007013, 4, 3,      i_trans},
  //! Overwritten in RV64I, analogous to sxxiw
  // {"slli",      0x00001013, 4, 3, shamt_trans},
  // {"srli",      0x00005013, 4, 3, shamt_trans},
  // {"srai",      0x40005013, 4, 3, shamt_trans},
  {"add",       0x00000033, 4, 3,      r_trans},
  {"sub",       0x40000033, 4, 3,      r_trans},
  {"sll",       0x00001033, 4, 3,      r_trans},
  {"slt",       0x00002033, 4, 3,      r_trans},
  {"sltu",      0x00003033, 4, 3,      r_trans},
  {"xor",       0x00004033, 4, 3,      r_trans},
  {"srl",       0x00005033, 4, 3,      r_trans},
  {"sra",       0x40005033, 4, 3,      r_trans},
  {"or",        0x00006033, 4, 3,      r_trans},
  {"and",       0x00007033, 4, 3,      r_trans},
  {"fence",     0x0000000F, 4, 2,  fence_trans}, 
  {"fence.tso", 0x8330000F, 4, 0,     no_trans},
  {"pause",     0x0100000F, 4, 0,     no_trans},
  {"ecall",     0x00000073, 4, 0,     no_trans},
  {"ebreak",    0x00100073, 4, 0,     no_trans},
  {"lwu",       0x00006003, 4, 3,      i_trans},
  {"ld",        0x00003003, 4, 3,      i_trans},
  {"sd",        0x00003023, 4, 3,      s_trans},
  {"slli",      0x00001013, 4, 3,  shamt_trans},
  {"srli",      0x00005013, 4, 3,  shamt_trans},
  {"srai",      0x40005013, 4, 3,  shamt_trans},
  {"addiw",     0x0000001B, 4, 3,      i_trans},
  {"slliw",     0x0000101B, 4, 3, shamtw_trans},
  {"srliw",     0x0000501B, 4, 3, shamtw_trans},
  {"sraiw",     0x4000501B, 4, 3, shamtw_trans},
  {"addw",      0x0000003B, 4, 3,      r_trans},
  {"subw",      0x4000003B, 4, 3,      r_trans},
  {"sllw",      0x0000103B, 4, 3,      r_trans},
  {"srlw",      0x0000503B, 4, 3,      r_trans},
  {"sraw",      0x4000503B, 4, 3,      r_trans},
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

instr_set RV64I = 
{
  instr_array,
  sizeof(instr_array)/sizeof(instr_def),
  reg_array,
  sizeof(reg_array)/sizeof(reg_def)
};