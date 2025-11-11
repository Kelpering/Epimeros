#include "private/directive.h"
#include "private/error.h"
#include "private/parsing.h"

//! Prevent directives from being saved within macro str_replace

void preprocessor_directive(
  const char* line,
  FILE* src_file, 
  FILE* dst_file, 
  macro_t* head
)  
{
  int line_pos = 0;
  char* directive = extract_delim(' ', line, &line_pos);

  throw_error("Extracted directive: \"%s\"", directive);

  return;
}