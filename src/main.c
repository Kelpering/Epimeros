#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "private/preprocessor.h"
#include "private/error.h"
#include "private/parsing.h"

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

  char* filename_buf = malloc(strlen(argv[1])+2);
  int i;
  for (i = 0; argv[1][i]; i++)
    filename_buf[i] = argv[1][i];
  filename_buf[i++] = '.';
  filename_buf[i++] = 'S';
  filename_buf[i++] = '\0';

  printf("New: %s\n", filename_buf);

  preprocess_file(argv[1], filename_buf);

  remove(filename_buf);
  return 0;
}