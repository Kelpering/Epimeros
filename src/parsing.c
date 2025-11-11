#include "private/parsing.h"
#include "private/error.h"
#include <errno.h>
#include <stdlib.h>

char skip_whitespace(const char* str, int* index)
{
  while (str[*index] == ' ')
    (*index)++;

  // Treat '\n' as a termination char (like '\0')
  char c = str[*index];
  if (c == '\n')
    c = '\0';

  return c;
}

char skip_until(char delim, const char* str, int* index)
{
  while (str[*index] != delim && str[*index] != '\n' && str[*index] != '\0')
    (*index)++;

  // Treat '\n' as a termination char (like '\0')
  char c = str[*index];
  if (c == '\n')
    c = '\0';

  return c;
}

char* extract_delim(char delim, const char* str, int* index)
{
  int t = *index;
  while (str[t] != delim && str[t] != '\n' && str[t] != '\0') {
    t++;
  }
  
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
  while (str[t] != ' ' && str[t] != '\n' && str[t] != '\0')
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
  // Return NULL if we encounter term char (Do not skip termchar)

  skip_whitespace(str, index);
  int t = *index;
  int excess_whitespace = 0;
  while (str[t] != ')' && str[t] != ',' && str[t] != '\n' && str[t] != '\0')
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
  *index += excess_whitespace + 1;

  

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