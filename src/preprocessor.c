#include "private/preprocessor.h"
#include "private/error.h"
#include "private/str_parsing.h"
#include "private/directive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//^ If we lean into the recursive nature of expand_macro
  //^ (using either a global or additional recurse_level variable)
//^ we might be able to improve the garbage code in macro processing

macro_t* init_macrolist()
{
  macro_t* macro_head = calloc(sizeof(macro_t), 1);

  // Creates special macros that are skipped
  alloc_macro("hi", NULL, -1, macro_head);
  alloc_macro("lo", NULL, -1, macro_head);
  alloc_macro("pcrel_hi", NULL, -1, macro_head);
  alloc_macro("pcrel_lo", NULL, -1, macro_head);

  return macro_head;
}

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
  // Expect an allocated but undefined macro at the end of the list.
  macro_t* curr = head;
  while (curr->macro != NULL)
  {
    if (strcmp(curr->macro, macro) == 0)
      throw_error("Duplicate macro definition");
    curr = curr->next;
  }

  // Handle sysmacro
  if (param_count == -1)
    goto sysmacro_skip;

  // Validate str_replace
  int str_pos = 0;
  while(str_replace[str_pos])
  {
    if (str_replace[str_pos] != '$')
    {
      str_pos++;
      continue;
    }
    
    // Correct over-extraction
    int pos_save = str_pos;
    char* op = extract_token(str_replace, &str_pos);
    int tmp = 1;
    while ('0' <= op[tmp] && op[tmp] <= '9')
      tmp++;
    op[tmp] = '\0';
    str_pos = pos_save + tmp;

    int64_t param_num = parse_int64_t(&op[1]);

    if (param_num <= 0 || param_num > param_count)
      throw_error("Macro arg \"%s\" is out of range.", op);
    
    free(op);
  }
  
  curr->str_replace = strdup(str_replace);
  sysmacro_skip:
  curr->macro = strdup(macro); 
  curr->param_count = param_count;
  curr->next = calloc(sizeof(macro_t), 1);

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
  
  macro_t* macro_def = search_macro(&macro[1], head);
  if (macro_def == NULL)
    throw_error("Undefined macro \"%s\"", macro);
  free(macro);

  // Handle sysmacro
  if (macro_def->param_count == -1)
  {
    char* temp = extract_delim(')', line, &line_pos);
    if (line[line_pos] != ')')
      throw_error("Malformed macro");
    fputc('%', dst_file);
    fputs(macro_def->macro, dst_file);
    fputc('(', dst_file);
    fputs(temp, dst_file);
    free(temp);
    return line_pos;
  }

  // Handle function params
  if (!func_macro && macro_def->param_count > 0)
    throw_error("Expected operands");
  else if (func_macro && macro_def->param_count <= 0)
    throw_error("Expected operands");

  // Parse all macro_args (including 0)
  char** macro_args = malloc(sizeof(char*) * macro_def->param_count);
  for (int i = 0; i < macro_def->param_count; i++)
  {
    macro_args[i] = extract_operand_strict(line, &line_pos);
    line_pos++;
    if (macro_args[i][0] == '%')
      throw_error("Macro argument forbidden");
  }
  line_pos--;
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
        // Correct over-extraction
        int i_save = i;
        char* token = extract_token(macro_def->str_replace, &i);
        int tmp = 1;
        while ('0' <= token[tmp] && token[tmp] <= '9')
          tmp++;
        token[tmp] = '\0';
        i = i_save + tmp;
        i--;  // for loop i++ must be accounted for.

        // Macro '$x' tokens are validated inside directive/alloc_macro
        fputs(macro_args[parse_uint64_t(&token[1]) - 1], dst_file);
        free(token);

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
    free(macro_args[i]);
  free(macro_args);
    
  return line_pos + 1;
}

FILE* preprocess_file(const char* path, const char* path_processed)
{
  //! Allow deletion of file by commenting
  // if (access(path_processed, F_OK) == 0)
  //   throw_error("File \"%s\" exists", path_processed);

  FILE* src_file = fopen(path, "r");
  if (src_file == NULL)
    throw_error("Cannot open \"%s\"", src_file);

  FILE* dst_file = fopen(path_processed, "w+");
  if (dst_file == NULL)
    throw_error("Cannot open \"%s\"", dst_file);

  //? Primary loop
  char line_buf[100];
  macro_t* macro_head = init_macrolist();
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

  while (macro_head->next)
  {
    free(macro_head->macro);
    free(macro_head->str_replace);
    
    macro_t* temp = macro_head->next;
    free(macro_head);
    macro_head = temp;
  }
  free(macro_head);

  
  fclose(src_file);
  rewind(dst_file);
  return dst_file;
}