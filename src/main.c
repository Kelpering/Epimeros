#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "private/preprocessor.h"
#include "private/error.h"
#include "private/parser.h"
#include "instr_set/RV32I.h"
#include "private/translation.h"

// #include "../old/include/"

//! OPERAND TYPES:
  //! Register: explanatory
  //! Immediate: Any explicitly-declared constant (including user macro)
  //! Label: Used to resolve forward-reference problems (mostly for pseudo-ops)
    //! Ex: j, li
  //! Macro: Used to resolve label problems (most instr. use partial label imm.)
    //! Ex: lui, auipc

int main(int argc, char** argv)
{
  if (argc != 2)
    throw_error("Incorrect number of arguments.\nExpected: 2; Provided: %d", 0, argc);

  char* filename_buf = calloc(strlen(argv[1]) + 3, 1);
  if (!filename_buf)
    throw_error("OoM");
  strcat(filename_buf, argv[1]);
  strcat(filename_buf, ".S");

  FILE* processed_file = preprocess_file(argv[1], filename_buf);
  free(filename_buf);

  parser_ctx* ctx = init_parserctx(0, 1, RV32I);

  instr_t* instr_list = parse_file(processed_file, ctx);
  temp_trans(instr_list, ctx);

  // Boilerplate free structure
  while(instr_list->next)
  {
    instr_t* temp = instr_list->next;
    for (int i = 0; i < instr_list->instr_def->op_count; i++)
      free(instr_list->op[i]);
    free(instr_list);
    instr_list = temp;
  }
  for (int i = 0; i < instr_list->instr_def->op_count; i++)
    free(instr_list->op[i]);
  free(instr_list);
  free_parserctx(ctx);
  
  
  fclose(processed_file);
  // remove(filename_buf);
  return 0;
}