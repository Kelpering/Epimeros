#ifndef __EPIMEROS_PARSER_H__
#define __EPIMEROS_PARSER_H__
#include <stddef.h>
#include <stdint.h>
#include "symtbl.h"

typedef struct parser_ctx parser_ctx;
typedef struct instr_def_t instr_def_t;
typedef struct instr_t instr_t;
typedef struct op_t op_t;

struct parser_ctx;
struct instr_def_t;
struct instr_t;
struct op_t;

typedef enum op_type
{
  REGISTER,
  IMMEDIATE,
  MACRO
} op_type;

struct op_t
{
  union
  {
    uint64_t u64;
    int64_t i64;
  } val;
  int64_t (*macro_cb)(int64_t label, parser_ctx* ctx);
  op_type type;
};

struct instr_t
{
    const instr_def_t* def;
    op_t* op;
    int op_size;
    instr_t* next;
};

struct parser_ctx;

struct instr_def_t
{ 
  char* mnemonic;
  uint32_t opcode;
  int byte_size;
  instr_t* (*parse_cb)(
    const char* line, 
    const instr_def_t* def, 
    parser_ctx* ctx
  );
  uint32_t (*trans_cb)(
    instr_t* instr, 
    parser_ctx* ctx
  );

};
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
  int starting_offset;
} parser_ctx;

parser_ctx* init_parserctx(instr_set* sets[],  reg_set* reg_sets[], int offset);
void free_parserctx(parser_ctx* ctx);

instr_t* parse_line(char* line, int byte_offset, parser_ctx* ctx);

// // All instruction types
// typedef enum
// {
//     // Pattern defines operand_t expected types/values
//     R_TYPE,             // rd, rs1, rs2
//     I_TYPE,             // rd, rs1, imm
//     S_TYPE,             // rs1, rs2, imm
//     B_TYPE,             // rs1, rs2, imm
//     U_TYPE,             // rd, imm
//     J_TYPE,             // rd, imm
//     NO_TYPE,            // Depends on mnemonic
//     ERROR_TYPE = -1     // Defines an error in parsing
// } instr_type;
// 
// // An enum to index into instr_defs
// typedef enum
// {
//     LUI,
//     AUIPC,
//     JAL,
//     JALR,
//     BEQ,
//     BNE,
//     BLT,
//     BGE,
//     BLTU,
//     BGEU,
//     LB,
//     LH,
//     LW,
//     LBU,
//     LHU,
//     SB,
//     SH,
//     SW,
//     ADDI,
//     SLTI,
//     SLTIU,
//     XORI,
//     ORI,
//     ANDI,
//     SLLI,
//     SRLI,
//     SRAI,
//     ADD,
//     SUB,
//     SLL,
//     SLT,
//     SLTU,
//     XOR,
//     SRL,
//     SRA,
//     OR,
//     AND,
//     FENCE,
//     ECALL,
//     EBREAK,
//     ERROR_MNEMONIC = -1
// } mnemonic_index;
// 
// // A definition of all valid instructions, mapped to mnemonic_index.
// static const struct
// {
//     const char* str;        // Mnemonic string
//     const uint32_t opcode;  // Partial opcode for valid instruction
//     const instr_type type;  // Associated instruction type
// } instr_defs[] = 
// {
//     [LUI]    = {"lui",     0b00000000000000000000000000110111, U_TYPE},
//     [AUIPC]  = {"auipc",   0b00000000000000000000000000010111, U_TYPE},
//     [JAL]    = {"jal",     0b00000000000000000000000001101111, J_TYPE},
//     [JALR]   = {"jalr",    0b00000000000000000000000001100111, I_TYPE},
//     [BEQ]    = {"beq",     0b00000000000000000000000001100011, B_TYPE},
//     [BNE]    = {"bne",     0b00000000000000000001000001100011, B_TYPE},
//     [BLT]    = {"blt",     0b00000000000000000100000001100011, B_TYPE},
//     [BGE]    = {"bge",     0b00000000000000000101000001100011, B_TYPE},
//     [BLTU]   = {"bltu",    0b00000000000000000110000001100011, B_TYPE},
//     [BGEU]   = {"bgeu",    0b00000000000000000111000001100011, B_TYPE},
//     [LB]     = {"lb",      0b00000000000000000000000000000011, I_TYPE},
//     [LH]     = {"lh",      0b00000000000000000001000000000011, I_TYPE},
//     [LW]     = {"lw",      0b00000000000000000010000000000011, I_TYPE},
//     [LBU]    = {"lbu",     0b00000000000000000100000000000011, I_TYPE},
//     [LHU]    = {"lhu",     0b00000000000000000101000000000011, I_TYPE},
//     [SB]     = {"sb",      0b00000000000000000000000000100011, S_TYPE},
//     [SH]     = {"sh",      0b00000000000000000001000000100011, S_TYPE},
//     [SW]     = {"sw",      0b00000000000000000010000000100011, S_TYPE},
//     [ADDI]   = {"addi",    0b00000000000000000000000000010011, I_TYPE},
//     [SLTI]   = {"slti",    0b00000000000000000010000000010011, I_TYPE},
//     [SLTIU]  = {"sltiu",   0b00000000000000000011000000010011, I_TYPE},
//     [XORI]   = {"xori",    0b00000000000000000100000000010011, I_TYPE},
//     [ORI]    = {"ori",     0b00000000000000000110000000010011, I_TYPE},
//     [ANDI]   = {"andi",    0b00000000000000000111000000010011, I_TYPE},
//     [SLLI]   = {"slli",    0b00000000000000000001000000010011, R_TYPE},
//     [SRLI]   = {"srli",    0b00000000000000000101000000010011, R_TYPE},
//     [SRAI]   = {"srai",    0b01000000000000000101000000010011, R_TYPE},
//     [ADD]    = {"add",     0b00000000000000000000000000110011, R_TYPE},
//     [SUB]    = {"sub",     0b01000000000000000000000000110011, R_TYPE},
//     [SLL]    = {"sll",     0b00000000000000000001000000110011, R_TYPE},
//     [SLT]    = {"slt",     0b00000000000000000010000000110011, R_TYPE},
//     [SLTU]   = {"sltu",    0b00000000000000000011000000110011, R_TYPE},
//     [XOR]    = {"xor",     0b00000000000000000100000000110011, R_TYPE},
//     [SRL]    = {"srl",     0b00000000000000000101000000110011, R_TYPE},
//     [SRA]    = {"sra",     0b01000000000000000101000000110011, R_TYPE},
//     [OR]     = {"or",      0b00000000000000000110000000110011, R_TYPE},
//     [AND]    = {"and",     0b00000000000000000111000000110011, R_TYPE},
//     [FENCE]  = {"fence",   0b0000'0000'0000'00000000000000001111, NO_TYPE},
//     [ECALL]  = {"ecall",   0b00000000000000000000000001110011, NO_TYPE},
//     [EBREAK] = {"ebreak",  0b00000000000100000000000001110011, NO_TYPE},
// };
// 
// // A definition of all valid registers.
// static const struct
// {
//     const char* str;            // Register String
//     const uint32_t reg_index;   // Register Opcode
// } reg_defs[] = 
// {
//     // Standard
//     {"x0",    0},
//     {"x1",    1},
//     {"x2",    2},
//     {"x3",    3},
//     {"x4",    4},
//     {"x5",    5},
//     {"x6",    6},
//     {"x7",    7},
//     {"x8",    8},
//     {"x9",    9},
//     {"x10",  10},
//     {"x11",  11},
//     {"x12",  12},
//     {"x13",  13},
//     {"x14",  14},
//     {"x15",  15},
//     {"x16",  16},
//     {"x17",  17},
//     {"x18",  18},
//     {"x19",  19},
//     {"x20",  20},
//     {"x21",  21},
//     {"x22",  22},
//     {"x23",  23},
//     {"x24",  24},
//     {"x25",  25},
//     {"x26",  26},
//     {"x27",  27},
//     {"x28",  28},
//     {"x29",  29},
//     {"x30",  30},
//     {"x31",  31},
// 
//     // ABI
//     {"zero", 0},
//     {"ra",   1},
//     {"sp",   2},
//     {"gp",   3},
//     {"tp",   4},
//     {"t0",   5},
//     {"t1",   6},
//     {"t2",   7},
//     {"s0",   8}, {"fp",   8},
//     {"s1",   9},
//     {"a0",  10},
//     {"a1",  11},
//     {"a2",  12},
//     {"a3",  13},
//     {"a4",  14},
//     {"a5",  15},
//     {"a6",  16},
//     {"a7",  17},
//     {"s2",  18},
//     {"s3",  19},
//     {"s4",  20},
//     {"s5",  21},
//     {"s6",  22},
//     {"s7",  23},
//     {"s8",  24},
//     {"s9",  25},
//     {"s10",  26},
//     {"s11",  27},
//     {"t3",  28},
//     {"t4",  29},
//     {"t5",  30},
//     {"t6",  31},
// };
// 
// typedef enum op_type
// {
//     REGISTER,   // Operand value is a reg_index from reg_defs
//     IMMEDIATE,  // Operand value is a verbatim immediate value
//     SPECIAL,    // Operand value is defined by mnemonic
// } op_type;
// 
// typedef struct operand_t
// {
//     op_type type;   // Type of operand
//     int64_t val;    // Value associated with operand
// } operand_t;
// 



// Mnemonics with a callback assigned for translation will always call.
    // Null for skip translation steps
// Most mnemonics will not need any further translation, so will
// also skip within the function

// Most likely translation requirement is going to be label handling.
// These can be easily handled with op_type checking and parsing.
    // If not already implemented, add an offset counter inside translator.c

// Custom callback functions will likely allow for much further opcode manip.
// Although there is a lack of "proprietary" information transfer support.
// They will likely have to use workarounds with the operands to transfer
// substantial data.
// 
// 
// instr_t parse_instruction(const char* str);
// 
#endif // __EPIMEROS_PARSER_H__
