#include <stdio.h>
#include "error.h"
#include "file.h"
#include "parser.h"
#include "parse_helper.h"
#include "instr_sets/RV32I.h"

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

    //! See translator.h (instr_t typedef) for more information.
  
  parser_ctx* ctx = init_parserctx(
    (instr_set*[]) {&RV32I, NULL}, 
    (reg_set*[])   {&RV32I_REGS, NULL}
  );

  parse_file(file, ctx);

  free_parserctx(ctx);
  fclose(file);

  return 0;
}

//^ Possible callback template
//^ Also, make all instructions have a callback to produce opcodes
// uint32_t instr_callback_template(instr_t* instr, symtbl, labl_offset)
// {
// 
// }