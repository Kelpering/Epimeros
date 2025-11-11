#include "parse_helper.h"
#include "error.h"
#include "sysmacro.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

parser_ctx* init_parserctx(instr_set* instr_sets[], 
  reg_set* reg_sets[], 
  int offset
)
{
  parser_ctx* ctx = malloc(sizeof(parser_ctx));
  ctx->symtbl = init_symtbl();
  ctx->starting_offset = offset;

  //? Parse instr_set
  int instr_set_count = 0;
  while (((void**)instr_sets)[instr_set_count])
    instr_set_count++;

  int instr_total = 0;
  for (int i = 0; i < instr_set_count; i++)
  {
    const instr_def_t* set =  *instr_sets[i];
    for (int j = 0; set[j].mnemonic; j++)
      instr_total++;
  }

  ctx->instr_count = instr_total;
  ctx->instr_list = malloc(sizeof(instr_def_t) * instr_total);
  
  int instr_ctr = 0;
  for (int i = 0; i < instr_set_count; i++)
  {
    const instr_def_t* set =  *instr_sets[i];
    for (int j = 0; set[j].mnemonic; j++)
    {
      ctx->instr_list[instr_ctr].mnemonic = strdup(set[j].mnemonic);
      ctx->instr_list[instr_ctr].byte_size = set[j].byte_size;
      ctx->instr_list[instr_ctr].opcode = set[j].opcode;
      ctx->instr_list[instr_ctr].parse_cb = set[j].parse_cb;
      ctx->instr_list[instr_ctr].trans_cb = set[j].trans_cb;

      for (int k = 0; ctx->instr_list[instr_ctr].mnemonic[k]; k++)
      {
        char c = ctx->instr_list[instr_ctr].mnemonic[k];
        if ('A' < c && c < 'Z')
          ctx->instr_list[instr_ctr].mnemonic[k] += 32;
      }

      instr_ctr++;
    }
  }

  for (int i = 0; i < ctx->instr_count; i++)
  {
    for (int j = i+1; j < ctx->instr_count; j++)
    {
      if (strcmp(ctx->instr_list[i].mnemonic, ctx->instr_list[j].mnemonic) == 0)
      {
        throw_error("Duplicate instruction definition \"%s\"", 
                    ctx->instr_list[i].mnemonic);
      }
    }
  }

  //? Parse reg_sets
  int reg_set_count = 0;
  while (((void**)reg_sets)[reg_set_count])
    reg_set_count++;

  int reg_total = 0;
  for (int i = 0; i < reg_set_count; i++)
  {
    const reg_def_t* set =  *reg_sets[i];
    for (int j = 0; set[j].str; j++)
      reg_total++;
  }

  ctx->reg_count = reg_total;
  ctx->reg_list = malloc(sizeof(reg_def_t) * reg_total);
  
  int reg_ctr = 0;
  for (int i = 0; i < reg_set_count; i++)
  {
    const reg_def_t* set =  *reg_sets[i];
    for (int j = 0; set[j].str; j++)
    {
      ctx->reg_list[reg_ctr].str = strdup(set[j].str);
      ctx->reg_list[reg_ctr].reg_index = set[j].reg_index;

      for (int k = 0; ctx->reg_list[reg_ctr].str[k]; k++)
      {
        char c = ctx->reg_list[reg_ctr].str[k];
        if ('A' < c && c < 'Z')
          ctx->reg_list[reg_ctr].str[k] += 32;
      }

      reg_ctr++;
    }
  }

  for (int i = 0; i < ctx->reg_count; i++)
  {
    for (int j = i+1; j < ctx->reg_count; j++)
    {
      if (strcmp(ctx->reg_list[i].str, ctx->reg_list[j].str) == 0)
      {
        throw_error("Duplicate register definition \"%s\"", 
                    ctx->reg_list[i].str);
      }
    }
  }

  return ctx;
}

void free_parserctx(parser_ctx* ctx)
{
  for (int i = 0; i < ctx->instr_count; i++)
    free(ctx->instr_list[i].mnemonic);
  free(ctx->instr_list);

  for (int i = 0; i < ctx->reg_count; i++)
    free(ctx->reg_list[i].str);
  free(ctx->reg_list);

  free_symtbl(ctx->symtbl);
  free(ctx);

  return;
}

char* extract_token(char c, const char* line, int* str_index)
{
  int t = *str_index;
  while (line[t] != c && line[t] != '\n' && line[t] != '\0')
    t++;
  t++;

  int token_size = t - *str_index;
  char* token = malloc(token_size);

  for (int i = 0; i < token_size; i++)
    token[i] = line[(*str_index)++];
  token[token_size-1] = '\0';

  return token;
}

op_t parse_reg(const char* line, int* str_index, parser_ctx* ctx)
{
  if (line[*str_index] == ',')
    throw_error("Empty operand");

  char* token = extract_token(',', line, str_index);

  int reg = -1;
  for (int i = 0; i < ctx->reg_count; i++)
  {
    if (strcmp(token, ctx->reg_list[i].str) == 0)
      reg = ctx->reg_list[i].reg_index;
  }
  free(token);

  if (reg == -1)
    throw_error("Unknown register");

  op_t op =
  {
    .val.u64 = reg,
    .type = REGISTER,
    .macro_cb = NULL
  };

  return op;
}

op_t parse_imm_macro(const char* token, parser_ctx* ctx)
{
  op_t op = 
  {
    .val.i64 = 0,
    .type = MACRO,
    .macro_cb = NULL
  };

  int str_index = 0;
  char* macro = extract_token('(', token, &str_index);
  char* label = extract_token(')', token, &str_index);

  if (strcmp("%hi", macro) == 0){
    op.val.i64 = search_symbol(label, ctx->symtbl);
    op.macro_cb = macro_hi;
  }
  else if (strcmp("%lo", macro) == 0) {
    op.val.i64 = search_symbol(label, ctx->symtbl);
    op.macro_cb = macro_lo;
  }
  else if (strcmp("%pcrel_hi", macro) == 0) {
    op.val.i64 = search_symbol(label, ctx->symtbl);
    op.macro_cb = macro_pcrel_hi;
  }
  else if (strcmp("%pcrel_lo", macro) == 0) {
    op.val.i64 = search_symbol(label, ctx->symtbl);
    op.macro_cb = macro_pcrel_lo;
  }

  return op;
}

op_t parse_imm(const char* line, int* str_index, parser_ctx* ctx)
{
  if (line[*str_index] == ',')
    throw_error("Empty Operand");

  char* token = extract_token(',', line, str_index);

  if (token[0] == '%')
    return parse_imm_macro(token, ctx);

  op_t op = 
  {
    .val.i64 = 0,
    .type = IMMEDIATE,
    .macro_cb = NULL
  };

  int token_size = 0;
  while(token[token_size])
    token_size++;

  int power = 1;
  int tok_index = 0;
  bool negative = false;

  if (token[tok_index] == '-')
  {
    negative = true;
    tok_index++;
  }

  // Different base processing
  if (token[tok_index] == '0')
  {
    tok_index++;
    switch (token[tok_index++]) 
    {
      case 'b':
        // Check valid characters
        for (int i = tok_index; token[i]; i++)
        {
          if (token[i] < '0' || token[i] > '1')
            throw_error("Unknown Immediate Symbol");
        }
          
        for (int i = token_size - 1; i >= tok_index; i--)
        {
          op.val.i64 += (token[i]-'0') * power;
          power *= 2;
        }

        if (negative)
          op.val.i64 = -(op.val.i64);
        free(token);
        return op;

      case 'x':
        // Check valid characters
        for (int i = tok_index; token[i]; i++)
        {
          if ((token[i] < '0' || token[i] > '9') &&
              (token[i] < 'a' || token[i] > 'f'))
          {
            throw_error("Unknown Immediate Symbol");
          }
        }
          
        for (int i = token_size - 1; i >= tok_index; i--)
        {
          int digit;
          if (token[i] >= 'a')
            digit = token[i] - 'a' + 10;
          else
            digit = token[i] - '0';   
          op.val.i64 += digit * power;
          power *= 16;
        }
          
        if (negative)
          op.val.i64 = -(op.val.i64);
        free(token);
        return op;
        
      default:
        tok_index -= 2;
        // Intentional fallthrough to default base 10 integer processing.
    }
  }

  // Check valid characters
  for (int i = tok_index; token[i]; i++)
  {
    if (token[i] < '0' || token[i] > '9')
      throw_error("Unknown Immediate Symbol");
  }

  for (int i = token_size - 1; i >= tok_index; i--)
  {
    op.val.i64 += (token[i]-'0') * power;
    power *= 10;
  }

  if (negative)
    op.val.i64 = -(op.val.i64);
  free(token);
  return op;
}
