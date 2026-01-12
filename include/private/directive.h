#ifndef __EPIMEROS_PRIVATE_DIRECTIVE_H__
#define __EPIMEROS_PRIVATE_DIRECTIVE_H__
#include "private/preprocessor.h"
#include <stdio.h>

void preprocessor_directive(
  const char* line, 
  FILE* src_file, 
  FILE* dst_file, 
  macro_t* head
);

#endif // __EPIMEROS_PRIVATE_DIRECTIVE_H__