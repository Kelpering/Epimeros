#include "private/str_parsing.h"
#include "private/error.h"
#include "private/symtbl.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

char skip_whitespace(const char* str, int* index)
{
  while (str[*index] == ' ')
    (*index)++;


  // Treat all delimiters as a termination char (like '\0')
  switch (str[*index]) 
  {
    case '\n':
    case '#':
      return '\0';
  }

  return str[*index];
}

char skip_until(char delim, const char* str, int* index)
{
  while (str[*index] != delim && str[*index] != '\n' && 
         str[*index] != '\0' && str[*index] != '#') {
    (*index)++;
  }

  // Treat all delimiters as a termination char (like '\0')
  switch (str[*index]) 
  {
    case '\n':
    case '#':
      return '\0';
  }

  return str[*index];
}

char* extract_delim(char delim, const char* str, int* index)
{
  int t = *index;
  while (str[t] != delim && str[t] != '\n' && str[t] != '\0' && str[t] != '#')
    t++;
  
  int token_size = t - *index;
  char* token = malloc(token_size + 1);
  
  for (int i = 0; i < token_size; i++)
    token[i] = str[(*index)++];
  token[token_size] = '\0';

  return token;
}

char* extract_token(const char* str, int* index)
{
  int t = *index;
  while (str[t] != ' ' && str[t] != '\n' && str[t] != '\0' && str[t] != '#')
    t++;
  
  int token_size = t - *index;
  char* token = malloc(token_size + 1);
  
  for (int i = 0; i < token_size; i++)
    token[i] = str[(*index)++];
  token[token_size] = '\0';

  return token;
}

char* extract_operand(const char* str, int* index)
{
  // Terminate on '\0' '\n' ')' ','

  skip_whitespace(str, index);
  int t = *index;
  int excess_whitespace = 0;
  while (str[t] != ')' && str[t] != ',' && str[t] != '\n' && 
         str[t] != '\0' && str[t] != '#')
  {
    if (str[t] == ' ')
    {
      excess_whitespace = t;
      char c = skip_whitespace(str, &t);
      if (c != ')' && c != ',' && c!= '\n' && c != '\0')
        throw_error("Malformed operand");
      
      // Removes unnecessary whitespace from token
      excess_whitespace = t - excess_whitespace;
      break;
    }
    t++;
  }

  // If there are no more operands, return a NULL ptr.
  int token_size = t - *index - excess_whitespace;
  if (token_size == 0)
    return NULL;
  char* token = malloc(token_size + 1);
  
  for (int i = 0; i < token_size; i++)
    token[i] = str[(*index)++];
  token[token_size] = '\0';
  *index += excess_whitespace;

  return token;
}

char* extract_operand_strict(const char* str, int* index)
{
  // Same as extract_operand
  // But throw error if it fails

  char* temp = extract_operand(str, index);
  if (!temp)
    throw_error("Expected operand");

  return temp;
}

int64_t parse_int64_t(char* str)
{
  errno = 0;
  char* end = str;
  int64_t num = strtoll(str, &end, 0);
  if (errno || *end != '\0')
  {
    if (errno == ERANGE)
      throw_error("Invalid integer (Out of Range) \"%s\"", str);

    throw_error("Invalid integer \"%s\"", str);
  }

  return num;
}

uint64_t parse_uint64_t(char* str)
{
  // Prevent strtoull underflow return val
  int str_pos = 0;
  if (skip_whitespace(str, &str_pos) == '-')
    throw_error("Invalid integer (Out of Range) \"%s\"", str);

  errno = 0;
  char* end = str;
  uint64_t num = strtoull(str, &end, 0);
  if (errno || *end != '\0')
  {
    if (errno == ERANGE)
      throw_error("Invalid integer (Out of Range) \"%s\"", str);
    
    throw_error("Invalid integer \"%s\"", str);
  }

  return num;
}

uint32_t parse_reg(char* str, parser_ctx* ctx)
{
  int64_t reg = -1;
  for (int i = 0; i < ctx->reg_count; i++)
  {
    if (strcmp(str, ctx->reg_def[i]->reg_str) == 0)
      reg = ctx->reg_def[i]->value;
  }
  if (reg == -1)
    throw_error("Unknown register");

  return (uint32_t) reg;
}

int64_t parse_imm(char* str, parser_ctx* ctx)
{
  //^ Fix this
  // int str_pos = 0;
  // char* macro = extract_delim('(', str, &str_pos);
  // str_pos++;
  // char* label = extract_operand_strict(str, &str_pos);
  int str_pos = 0;
  char* imm = extract_operand_strict(str, &str_pos);
  
  //? Integer immediate
  if (('0' <= imm[0] && imm[0] <= '9') || imm[0] == '-')
  {
    int64_t ret = parse_int64_t(imm);
    free(imm);
    return ret;
  }

  //? Label immediate
  if (imm[0] != '%')
  {
    symbol_t* sym = search_symtbl_strict(imm, ctx->symtbl);
    free(imm);
    return sym->byte_offset;
  }

  //? Macro immediate
  free(imm);
  str_pos = 0;
  skip_whitespace(str, &str_pos);
  char* macro = extract_delim('(', str, &str_pos);
  str_pos++;
  imm = extract_operand_strict(str, &str_pos);

  int64_t val;
  if (('0' <= imm[0] && imm[0] <= '9') || imm[0] == '-')
    val = parse_int64_t(imm);
  else
    val = search_symtbl_strict(imm, ctx->symtbl)->byte_offset;
  free(imm);

  //! There is a slight issue with these macros
  //! Since addi sign-extends the final bit, we need to compensate
  //! if %lo is > 0x800
  //! So we use negative %lo and rounded %hi
  
  //! The question is, is it ok to do this for ALL %hi/%lo (even unpaired)?
  //! They ARE meant to be used in pairs, and there is little utility in
  //! only using one (unless under specific circumstances)
  
  //! And since %lo must be greater than a value, that makes it guaranteed
  //! to be using both (otherwise the number would be off anyway)

  //! Extra assumption, since pcrel hi/lo are used in tandem
    //! And the PC will vary between the two lines
  //! Should we naturally account for that as well? (+4 to pc for pcrel_hi idk)
  if (strcmp("%hi", macro) == 0)
  {
    free(macro);
    return val & 0xFFFFF000;
  }
  else if (strcmp("%lo", macro) == 0)
  {
    free(macro);
    return val & 0x00000FFF;
  }
  else if (strcmp("%pcrel_hi", macro) == 0)
  {
    free(macro);
    throw_error("PCREL_HI MACRO");
  }
  else if (strcmp("%pcrel_lo", macro) == 0)
  {
    free(macro);
    throw_error("PCREL_LO MACRO");
  }

  throw_error("BYE MACRO"); 
}

uint32_t process_bit_range(int64_t value, int bitsize, bool sign)
{
  if (bitsize > 32)
    throw_error("Bitsize too large");

  if (sign && (value < -(1L<<(bitsize-1)) || value > ((1L<<(bitsize-1)) - 1)))
    throw_error("Integer out of range\nValid range: %d - %d\nActual: %ld", 
                -(1L<<(bitsize-1)), 
                ((1L<<(bitsize-1)) - 1), 
                value);
  if (!sign && (value < 0 || value > (1L<<bitsize)))
    throw_error("Integer out of range\nValid range: %d-%d\nActual: %ld", 
                0, 
                ((1L<<bitsize) - 1), 
                value);

  return (uint32_t) value;
}