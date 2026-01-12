#include "private/directive.h"
#include "private/error.h"
#include "private/str_parsing.h"
#include "private/preprocessor.h"
#include <stdlib.h>
#include <string.h>

void directive_macro(
  const char* line,
  FILE* src_file, 
  FILE* dst_file, 
  macro_t* head
)  
{
  int line_pos = 0;

  skip_whitespace(line, &line_pos);
  char* macro_name = extract_token(line, &line_pos);
  if (macro_name[0] == '\0')
    throw_error("Missing macro name in .macro directive");

  skip_whitespace(line, &line_pos);
  char* param_token = extract_token(line, &line_pos);
  int param_count;
  if (param_token[0] == '\0')
    param_count = 0;
  else
    param_count = parse_uint64_t(param_token);
  free(param_token);

  char c = skip_whitespace(line, &line_pos);
  if (c != '\0' && c != '#')
    throw_error("Excess operands");

  
  int macro_pos = 0;
  char macro_buf[1000] = {0};
  // char tmp_buf[100];
  while(fgets(&macro_buf[macro_pos], sizeof(macro_buf) - macro_pos, src_file))
  {
    // Increment global line_num (for error handling)
    line_num++;
    
    // Detect buffer size and prevent overflow.
    int curr_pos = 0;
    int buf_size = 0;
    while (macro_buf[macro_pos + buf_size])
    {
      // Replace '#' with '\0' to remove comments
      if (macro_buf[macro_pos + buf_size] == '#')
        macro_buf[macro_pos + buf_size] = '\0';
      
      buf_size++;
    }

    if (macro_buf[macro_pos + buf_size - 1] != '\n' && !feof(src_file))
      throw_error("Macro buffer overflow");

    switch (skip_whitespace(&macro_buf[macro_pos], &curr_pos)) 
    {
      case '.':
        char* directive = extract_delim(' ', &macro_buf[macro_pos], &curr_pos);
        if (strcmp(directive, ".macro") == 0)
          throw_error("Recursive \".macro\" directive");
        if (strcmp(directive, ".endm") != 0)
        {
          free(directive);
          break;
        }
        free(directive);


        // .endm
        if (skip_whitespace(&macro_buf[macro_pos], &curr_pos) != '\0')
          throw_error("Excess operands");
        macro_buf[macro_pos - 1] = '\0';   // Remove final '\n'
        alloc_macro(macro_name, macro_buf, param_count, head);
        free(macro_name);
        return;
    }

    macro_pos += buf_size;
  }

  throw_error("Unmatched .macro directive");
}

void preprocessor_directive(
  const char* line,
  FILE* src_file, 
  FILE* dst_file, 
  macro_t* head
)  
{
  int line_pos = 0;
  char* directive = extract_delim(' ', line, &line_pos);

  if (strcmp(directive, ".macro") == 0)
  {
    free(directive);
    directive_macro(&line[line_pos], src_file, dst_file, head);
    return;
  }

  if (strcmp(directive, ".endm") == 0)
    throw_error("Unmatched .endm directive");

  throw_error("Unknown directive \"%s\"", directive);
}