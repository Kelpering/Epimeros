#include "private/parser.h"
#include "private/str_parsing.h"
#include "private/symtbl.h"
#include "private/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

parser_ctx* init_parserctx(uint64_t start_addr, int set_count, ...)
{
  parser_ctx* ctx = malloc(sizeof(parser_ctx));
  ctx->offset = start_addr;
  ctx->symtbl = init_symtbl();

  va_list vargs;
  va_start(vargs, set_count);
  instr_set curr_set;
  ctx->instr_count = 0;
  ctx->reg_count = 0;
  for (int i = 0; i < set_count; i++)
  {
    curr_set = va_arg(vargs, instr_set);
    ctx->instr_count += curr_set.instr_def_size;
    ctx->reg_count += curr_set.reg_def_size;
  }
  va_end(vargs);

  ctx->instr_def = malloc(sizeof(instr_def*) * ctx->instr_count);
  ctx->reg_def = malloc(sizeof(reg_def*) * ctx->reg_count);

  // Restart va_list
  va_start(vargs, set_count);
  int curr_instr = 0;
  int curr_reg = 0;
  
  // This entire block is 3 simple loops (duplicated for reg_defs later)
    // per set
    // per defined instruction within a set
    // per currently defined (parser_ctx) instruction
  for (int set_i = 0; set_i < set_count; set_i++)
  {
    curr_set = va_arg(vargs, instr_set);
    for (int def_j = 0; def_j < curr_set.instr_def_size; def_j++)
    {
      // Prevent double definitons
      for (int curr_k = 0; curr_k < curr_instr; curr_k++)
      {
        if (strcmp(ctx->instr_def[curr_k]->mnemonic, 
                   curr_set.instr_def_array[def_j].mnemonic) == 0)
        {
          throw_error("Double instruction mnemonic definition \"%s\"", 
                      ctx->instr_def[curr_k]->mnemonic);
        }
      }
      ctx->instr_def[curr_instr++] = &curr_set.instr_def_array[def_j];
    }
    for (int j = 0; j < curr_set.reg_def_size; j++)
    {
      // Prevent double definitons
      for (int k = 0; k < curr_reg; k++)
      {
        if ((strcmp(ctx->reg_def[k]->reg_str, 
                   curr_set.reg_def_array[j].reg_str) == 0) &&
            ctx->reg_def[k]->value != curr_set.reg_def_array[j].value)
        {
          throw_error("Double instruction mnemonic definition \"%s\"", 
                      ctx->reg_def[k]->reg_str);
        }
      }
      ctx->reg_def[curr_reg++] = &curr_set.reg_def_array[j];
    }
  }
  va_end(vargs);

  return ctx;
}

void free_parserctx(parser_ctx* ctx)
{
  free(ctx->instr_def);
  free(ctx->reg_def);
  free_symtbl(ctx->symtbl);
  free(ctx);

  return;
}

instr_def* search_instr_defs(char* mnemonic, parser_ctx* ctx)
{
  for (int i = 0; i < ctx->instr_count; i++)
  {
    if (strcmp(mnemonic, ctx->instr_def[i]->mnemonic) == 0)
      return ctx->instr_def[i];
  }

  throw_error("Unknown mnemonic \"%s\"", mnemonic);
}

instr_t* parse_line(char* line, parser_ctx* ctx)
{
  
  // Validate letters
  for (int i = 0; line[i]; i++)
  {
    char c = line[i];

    // Alphabetic
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
      continue;

    // Numeric
    if ('0' <= c && c <= '9')
      continue;
  
    // Special
    switch (c)
    {
      case ' ':
      case '%':
      case '(':
      case ')':
      case ',':
      case '-':
      case '.':
      case ':':
      case '_':
        continue;

      // End of Line
      case '\0':
      case '\n':
      case '#':
        line[i] = '\0';
        break;

      default:
        throw_error("Invalid character \'%c\'", c);
    }
  }

  int line_pos = 0;

  // Skip pre-letter whitespace
  if (skip_whitespace(line, &line_pos) == '\0')
    return NULL;

  // Check for label count
  int label_count = 0;
  for (int i = 0; line[i]; i++)
  {
    if (line[i] == ':')
      label_count++;
  }
  if (label_count > 1)
    throw_error("Too many labels on one line");

  // Label processing
  if (label_count)
  {
    char* label = extract_delim(':', line, &line_pos);
    line_pos++;

    // Validate label
    for (int i = 0; label[i]; i++)
    {
      if (label[i] == ' ' && skip_whitespace(label, &i) != '\0')
        throw_error("Split label");

      switch (label[i]) 
      {
        // Forbid characters from label (we parsed line earlier for other chars)
        case '(':
        case ')':
        case ',':
          throw_error("Invalid char in label \'%c\'", label[i]);

        default:
          break;
      }
    }

    // Case sensitive label
    alloc_symbol(label, ctx->offset, ctx->symtbl);
    free(label);
  }

  // Skip pre-letter whitespace
  if (skip_whitespace(line, &line_pos) == '\0')
    return NULL;

  // Mnemonic processing
  char* mnemonic = extract_token(line, &line_pos);
  for (int i = 0; mnemonic[i]; i++)
  {
    if ('A' <= mnemonic[i] && mnemonic[i] <= 'Z')
      mnemonic[i] += 32;
  }



  instr_def* def = search_instr_defs(mnemonic, ctx);
  ctx->offset += def->byte_width;
  instr_t* instr = malloc(sizeof(instr_t) + sizeof(char*) * def->op_count);
  instr->instr_def = def;
  instr->next = NULL;
  free(mnemonic);
  

  // Early return for no operands (skip parse_cb)
  if (skip_whitespace(line, &line_pos) == '\0')
  {
    if (def->op_count == 0)
     return instr;
    else
      throw_error("Expected operand");
  }

  if (def->op_count)
  {
    for (int i = 0; i < def->op_count - 1; i++)
    {
      //! Doesnt check split tokens
      //! Extract operand experiences errors with ), (Since both are termchar)
      skip_whitespace(line, &line_pos);
      instr->op[i] = extract_delim(',', line, &line_pos);
      {
        // emergency (portfolio) whitespace fixing (Was WIP fix)
        int pos = strlen(instr->op[i]) - 1;
        while (instr->op[i][pos] == ' ')
        {
          instr->op[i][pos] = '\0';
          pos--;
        }
      }
      line_pos++;
    }
    skip_whitespace(line, &line_pos);
    instr->op[def->op_count-1] = extract_delim('\n', line, &line_pos);
    {
      // emergency (portfolio) whitespace fixing (Was WIP fix)
      int pos = strlen(instr->op[def->op_count-1]) - 1;
      while (instr->op[def->op_count-1][pos] == ' ')
      {
        instr->op[def->op_count-1][pos] = '\0';
        pos--;
      }
    }
  }
  if (skip_whitespace(line, &line_pos) != '\0')
    throw_error("Excess operand");

  return instr;
}

instr_t* parse_file(FILE* file, parser_ctx* ctx)
{
  line_num = 0;

  char line_buf[100];
  instr_t* head = NULL;
  instr_t* prev = &(instr_t){NULL};
  while (fgets(line_buf, sizeof(line_buf), file))
  {
    line_num++;

    instr_t* curr = parse_line(line_buf, ctx);
    if (!head)
      head = curr;
    
    // Skips empty lines/instructions
    if (curr)
    {
      prev->next = curr;

      // Allows parse_line to return more than one instruction
      // Helpful for psuedo-instructions (like li)
      while (prev->next)
        prev = prev->next;
    }
  }

  return head;
}
