#include "private/preprocessor.h"
#include "private/error.h"
#include "private/parsing.h"
#include "private/directive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

macro_t* search_macro(const char* macro, macro_t* head)
{
  macro_t* curr = head;
  while (curr->macro)
  {
    if (strcmp(macro, curr->macro) == 0)
      return curr;

    curr = curr->next;
  }

  return NULL;
}

macro_t* alloc_macro(
  const char* macro, 
  const char* str_replace, 
  int param_count, 
  macro_t* head
)
{
  // Validate str_replace
  int str_pos = 0;
  while(str_replace[str_pos])  
  {
    if (str_replace[str_pos] != '$')
    {
      str_pos++;
      continue;
    }
    
    char* op = extract_token(str_replace, &str_pos);
    int64_t param_num = parse_int64_t(&op[1]);

    if (param_num <= 0 || param_num > param_count)
      throw_error("Macro arg \"%s\" is out of range.", op);
    
    free(op);
  }

  // Expect an allocated but undefined macro at the end of the list.
  macro_t* curr = head;
  while (curr->macro != NULL)
    curr = curr->next;

  curr->macro = strdup(macro);
  curr->str_replace = strdup(str_replace);
  curr->param_count = param_count;
  curr->next = malloc(sizeof(macro_t));

  return curr;
}

int expand_macro(const char* line, macro_t* head, FILE* dst_file, int lvl)
{
  if (lvl > 10)
    throw_error("Too many recursive macro calls, possible infinite loop.");

  // Write find and replace macro into dst_file
  // Expand macros within macros in here
  // Verify that $1 $2... all match during allocation

  int t = 0;
  while (line[t] != ' ' && line[t] != '(' && line[t] != '\n' && line[t] != '\0')
    t++;

  int line_pos = 0;
  bool func_macro;
  char* macro;
  if (line[t] == ' ' || line[t] == '\n' || line[t] == '\0') 
  {
    func_macro = false;
    macro = extract_delim(' ', line, &line_pos);
  }
  else if (line[t] == '(')
  {
    func_macro = true;
    macro = extract_delim('(', line, &line_pos);
    line_pos++;
  }
  else
  {
    throw_error("Malformed macro \"%s\"", extract_token(line, &line_pos));
  }
  
  macro_t* macro_def = search_macro(macro, head);
  if (macro_def == NULL)
    throw_error("Undefined macro \"%s\"", macro);

  // Handle function params
  if (!func_macro && macro_def->param_count > 0)
    throw_error("Expected operands");
  else if (func_macro && macro_def->param_count <= 0)
    throw_error("Expected operands");

  // Parse all macro_args (including 0)
  char** macro_args = malloc(sizeof(char*) * macro_def->param_count);
  for (int i = 0; i < macro_def->param_count; i++)
    macro_args[i] = extract_operand_strict(line, &line_pos);
  if (macro_def->param_count && extract_operand(line, &line_pos))
    throw_error("Excess operand");

  //^ Check for macro arguments (which are forbidden?)
  //^ Generally tweak the off by one errors (Spaces, etc are being cutoff)
  //^ Thoroughly check the macro parser below

  for (int i = 0; macro_def->str_replace[i]; i++)
  {
    switch (macro_def->str_replace[i]) 
    {
      case '$':
        // Skip '$', then don't skip the parsed token char.
        i++;
        char* token = extract_token(macro_def->str_replace, &i);
        i--;

        // Macro '$x' tokens are validated inside directive/alloc_macro
        fputs(macro_args[parse_uint64_t(token) - 1], dst_file);
        break;

      case '%':
        i += expand_macro(&macro_def->str_replace[i],head, dst_file, lvl+1);
        i--;
        break;

      default:
        fputc(macro_def->str_replace[i], dst_file);
    }
  }

  for (int i = 0; i < macro_def->param_count; i++)
  {
    printf("TEST PARAM: \"%s\"\n", macro_args[i]);
  }

  return line_pos;
}




// 
// void macro_directive(
//   char* line_buf, 
//   symtbl_t* macro_symtbl, 
//   FILE* src_file, 
//   FILE* dst_file
// )
// {
//   int line_pos = 0;
//   skip_whitespace(line_buf, &line_pos);
//   char* macro_name = extract_token(line_buf, &line_pos);
// 
//   // If there are excess chars to process on this line, throw error.
//   if (skip_whitespace(line_buf, &line_pos))
//     throw_error("Excess directive operands");
// 
//   //! Does not handle commas
//     //^ extract_term_token (' ', ',', '\n', '\0')
//   //! Function macros
//     //^ Skip whitespace
//   //! 
// 
//   int macro_pos = 0;
//   char macro_buf[1000] = {0};
//   char tmp_buf[100];
//   while (fgets(tmp_buf, sizeof(tmp_buf), src_file))
//   {
//     line_num++;
//     
//     // Check tmp_buf size and overflow
//     int tmp_size = 0;
//     while (tmp_buf[tmp_size])
//       tmp_size++;
//     if (tmp_buf[tmp_size-1] != '\n' && !feof(src_file))
//       throw_error("Line buffer overflow");
// 
//     // Check for directive
//     int line_pos = 0;
//     if (skip_whitespace(line_buf, &line_pos) == '.')
//     {
//       char* tok = extract_token(tmp_buf, &line_pos);
// 
//       if (strcmp(".macro", tok) == 0)
//         throw_error("Recursive \".macro\" directive");
//       if (strcmp(".endm", tok) != 0)
//       {
//         free(tok);
//         goto skip_directive;
//       }
//       free(tok);
// 
//       // Overwrite final '\n'
//       macro_buf[macro_pos - 1] = '\0';
//       
//       alloc_symbol(macro_name, (uint64_t)strdup(macro_buf), macro_symtbl);
//       return;
//     }
//     skip_directive:
//     
//     if (tmp_size + macro_pos + 1 > 1000)
//       throw_error("Macro buffer overflow");
//     
//     // Append tmp_buf to macro_buf (including '\n')
//     for (int i = 0; i < tmp_size; i++)
//       macro_buf[macro_pos++] = tmp_buf[i];
//   }
//   throw_error("Unmatched \".macro\" directive");
// 
//   return;
// }
// 
// void expand_macro(const char* line, symtbl_t* macro_symtbl, FILE* dst_file)
// {
//   printf("line: \"%s\"", line);
// 
//   throw_error("END EXECUTION");
// 
//   const char* macro = 0;
// 
//   uint32_t sym_index = search_symtbl(macro, macro_symtbl);
// 
//   if (macro_symtbl->sym[sym_index].type != SYM_ALLOCATED)
//     throw_error("Undefined macro \"%%%s\"", macro);
// 
//   int i = 0;
//   while(macro_symtbl->sym[sym_index].val.str_ptr[i])
//   {
//     if (macro_symtbl->sym[sym_index].val.str_ptr[i] == '%')
//     {
// 
//       continue;
//     }
// 
//     fputc(macro_symtbl->sym[sym_index].val.str_ptr[i], dst_file);
//   }
// 
//   // fputs(, dst_file);
// }
// 
// void process_user_macros(FILE* src_file, FILE* dst_file)
// {
//   // We use macro_symtbl to collect macro definitions. Since macros must be
//   // pre-defined before use, we can throw error on SYM_EXPECTED.
//   symtbl_t* macro_symtbl = init_symtbl();
//   //* Create macro table
//   //* Macros must be predefined before use
//   //* Read line by line
//   //* Processing within function
//   //! Macros must be resolved per operand, not per symbol
// 
//   char line_buf[100];
//   while (fgets(line_buf, sizeof(line_buf), src_file))
//   {
//     line_num++;
// 
//     // Detect line buffer overflow
//     int len = 0;
//     while (line_buf[len++]);
//     len--;
//     if (line_buf[len-1] != '\n' && !feof(src_file))
//       throw_error("Line buffer overflow");
// 
//     // Check for directive
//     int line_pos = 0;
//     if (skip_whitespace(line_buf, &line_pos) == '.')
//     {
//       char* tok = extract_token(line_buf, &line_pos);
//       if (line_buf[line_pos] != ' ')
//         throw_error("Malformed .macro directive");
// 
//       if (strcmp(".endm", tok) == 0)
//         throw_error("Unmatched \".endm\" directive");
//       if (strcmp(".macro", tok) != 0)
//       {
//         free(tok);
//         goto skip_directive;
//       }
// 
//       macro_directive(&line_buf[line_pos], macro_symtbl, src_file, dst_file);
//       continue;
//     }
//     skip_directive:
//     
//     // Parse line for user macros
//     while(line_buf[line_pos])
//     {
//       if (line_buf[line_pos] != '%')
//       {
//         fputc(line_buf[line_pos++], dst_file);
//         continue;
//       }
// 
//       expand_macro(&line_buf[line_pos++], macro_symtbl, dst_file);
//     }
//   }
// 
//   return;
// }
// 
FILE* preprocess_file(const char* path, const char* path_processed)
{
  //! Temporary comment (Will destroy file)
//   if (access(path_processed, F_OK) == 0)
//     throw_error("File \"%s\" exists", path_processed);

  FILE* src_file = fopen(path, "r");
  if (src_file == NULL)
    throw_error("Cannot open \"%s\"", src_file);

  FILE* dst_file = fopen(path_processed, "w");
  if (dst_file == NULL)
    throw_error("Cannot open \"%s\"", dst_file);

  //? Primary loop
  char line_buf[100];
  macro_t* macro_head = malloc(sizeof(macro_t));
  alloc_macro("%TEST", "YEST", 0, macro_head);
  alloc_macro("%TESTMACRO", "THIS %TEST IS A TEST\n $1 $2 $3\n", 3, macro_head);
  while (fgets(line_buf, sizeof(line_buf), src_file))
  {
    // Increment global line_num (for error handling)
    line_num++;

    // Detect line buffer overflow
    int line_size = 0;
    
    while (line_buf[line_size++]);
    line_size--;
    if (line_buf[line_size-1] != '\n' && !feof(src_file))
      throw_error("Line buffer overflow");

    int line_pos = 0;

    // Process special first chars
    switch(skip_whitespace(line_buf, &line_pos))
    {
      case '\0':  // Early exit for blank lines
      case '#':   // Early exit for comment lines
        continue;

      case '.':
        // Directives consume an entire line, so we can exit early here as well
        preprocessor_directive(
          &line_buf[line_pos], 
          src_file, dst_file, 
          macro_head
        );
        continue;
    }

    // Parse line for macros
    while(line_buf[line_pos])
    {
      // Ignore comment lines (also removes them from dst_file)
      if (line_buf[line_pos] == '#')
        break;

      if (line_buf[line_pos] != '%')
      {
        fputc(line_buf[line_pos++], dst_file);
        continue;
      }

      line_pos += expand_macro(&line_buf[line_pos], macro_head, dst_file, 0);
    }


  }

  fclose(src_file);
  rewind(dst_file);
  return dst_file;
}