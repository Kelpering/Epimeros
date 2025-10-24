#include <stdio.h>
#include "error.h"
#include "file.h"
#include "parser.h"
#include "parse_helper.h"

//^ TODO:
//^     Finish Labels (Mnemonic OR Label, not both)
//^     File Parser (w/ line support)

// We are going to convert this into a "runtime register" architecture
// All mnemonics are registered during an init() function.
    //? We might be able to register them mid-operation.
// Mnemonics would call the pre-defined callback functions (default types)
// OR
// Mnemonics would call a custom callback function to handle more complex code.
    //? They could also use a library to simplify parsing (next_op, error, etc)

//* Pseudo-Ops [Custom Mnemonics]
//^ Macros     [String Replacement?]
//^ Directives [Something Translation?]

int main(int argc, char** argv)
{
  if (argc != 2)
    throw_error("Incorrect number of arguments.\nExpected: 2; Provided: %d", 0, argc);

  FILE* file = fopen(argv[1], "r");

  if (file == NULL)
    throw_error("File not found.", 0);

  instr_set RV32I = 
  {
    {"lui",     0b00000000000000000000000000110111, 4, U_PARSE},
    {"auipc",   0b00000000000000000000000000010111, 4, U_PARSE},
    {"jal",     0b00000000000000000000000001101111, 4, J_PARSE},
    {"jalr",    0b00000000000000000000000001100111, 4, I_PARSE},
    {"beq",     0b00000000000000000000000001100011, 4, B_PARSE},
    {"bne",     0b00000000000000000001000001100011, 4, B_PARSE},
    {"blt",     0b00000000000000000100000001100011, 4, B_PARSE},
    {"bge",     0b00000000000000000101000001100011, 4, B_PARSE},
    {"bltu",    0b00000000000000000110000001100011, 4, B_PARSE},
    {"bgeu",    0b00000000000000000111000001100011, 4, B_PARSE},
    {"lb",      0b00000000000000000000000000000011, 4, I_PARSE},
    {"lh",      0b00000000000000000001000000000011, 4, I_PARSE},
    {"lw",      0b00000000000000000010000000000011, 4, I_PARSE},
    {"lbu",     0b00000000000000000100000000000011, 4, I_PARSE},
    {"lhu",     0b00000000000000000101000000000011, 4, I_PARSE},
    {"sb",      0b00000000000000000000000000100011, 4, S_PARSE},
    {"sh",      0b00000000000000000001000000100011, 4, S_PARSE},
    {"sw",      0b00000000000000000010000000100011, 4, S_PARSE},
    {"addi",    0b00000000000000000000000000010011, 4, I_PARSE},
    {"slti",    0b00000000000000000010000000010011, 4, I_PARSE},
    {"sltiu",   0b00000000000000000011000000010011, 4, I_PARSE},
    {"xori",    0b00000000000000000100000000010011, 4, I_PARSE},
    {"ori",     0b00000000000000000110000000010011, 4, I_PARSE},
    {"andi",    0b00000000000000000111000000010011, 4, I_PARSE},
    {"slli",    0b00000000000000000001000000010011, 4, R_PARSE},
    {"srli",    0b00000000000000000101000000010011, 4, R_PARSE},
    {"srai",    0b01000000000000000101000000010011, 4, R_PARSE},
    {"add",     0b00000000000000000000000000110011, 4, R_PARSE},
    {"sub",     0b01000000000000000000000000110011, 4, R_PARSE},
    {"sll",     0b00000000000000000001000000110011, 4, R_PARSE},
    {"slt",     0b00000000000000000010000000110011, 4, R_PARSE},
    {"sltu",    0b00000000000000000011000000110011, 4, R_PARSE},
    {"xor",     0b00000000000000000100000000110011, 4, R_PARSE},
    {"srl",     0b00000000000000000101000000110011, 4, R_PARSE},
    {"sra",     0b01000000000000000101000000110011, 4, R_PARSE},
    {"or",      0b00000000000000000110000000110011, 4, R_PARSE},
    {"and",     0b00000000000000000111000000110011, 4, R_PARSE},
    {"fence",   0b00000000000000000000000000001111, 4, NO_PARSE}, //! Custom
    {"ecall",   0b00000000000000000000000001110011, 4, NO_PARSE},
    {"ebreak",  0b00000000000100000000000001110011, 4, NO_PARSE},
    NULL
    };

    reg_set RV32I_REGS = 
    {
      {"x0",  0},
      {"x1",  1},
      NULL
    };

    reg_set RV64I_REGS = 
    {
      {"x2",  2},
      {"x3",  3},
      NULL
    };

  //^ We need to reorganize the entire parse_helper and parser headers
  //^ Current idea:
    //^ We initialize parser_ctx with any instruction and register sets required
    //^ for operation. Instruction sets contain a list of definitions.
    //^ Each definition has a mnemonic string, partial opcode, byte size,
    //^ and a callback function.
    
    //^ The callback function will create an instruction that we can save.
    //^ Then the callback can add a translation callback to the instr_t*
    //^ This callback will be executed during translation, allowing for
    //^ seamless label parsing.

    //^ Overall, the project has to be reorganized.
    //^ Public facing: init_parserctx, parse_file, etc
    //^ Dev facing: parse_helper.h, instr_set definiton, etc
    //^ Private: symtbl, parse_line, translation, etc
    //^ Public idea: instr_set.h/reg_set.h or std_sets.h
  
  parser_ctx* ctx = init_parserctx(
    (instr_set*[]) {&RV32I, NULL}, 
    (reg_set*[])   {&RV32I_REGS, &RV64I_REGS, NULL}
  );

  // parse_file(file, ctx);

  free_parserctx(ctx);
  fclose(file);

  return 0;
}