#include "parser.h"
#include "symtbl.h"
#include <stdlib.h>
#include <string.h>
#include "error.h"

#include <stdio.h>

parser_ctx* init_parserctx(instr_set* instr_sets[], reg_set* reg_sets[])
{
  parser_ctx* ctx = malloc(sizeof(parser_ctx));
  ctx->symtbl = init_symtbl();

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

char* extract_token(const char* line, int* str_index)
{
  int t = *str_index;
  while (line[t] != ',' && line[t] != '\n' && line[t] != '\0')
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

  char* token = extract_token(line, str_index);

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
    .type = REGISTER
  };

  return op;
}

op_t parse_int(const char* token, parser_ctx* ctx)
{
  op_t op = 
  {
    .val.i64 = 0,
    .type = IMMEDIATE
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

op_t parse_label(const char* line, parser_ctx* ctx)
{
  throw_error("LABEL NOT IMPLEMENTED (parse_helper.c)");
}

op_t parse_imm(const char* line, int* str_index, parser_ctx* ctx)
{
  if (line[*str_index] == ',')
    throw_error("Empty Operand");

  char* token = extract_token(line, str_index);

  if (('0' <= token[0] && token[0] <= '9') || token[0] == '-')
    return parse_int(token, ctx);
  else
    return parse_label(token, ctx);
}


// 
// int parse_reg(const char* norm_str, int* str_index)
// {
//     if (norm_str[*str_index] == ',')
//         throw_error("Empty Operand");
//     
//     int tok_index = *str_index;
//     while (norm_str[tok_index] != ',' && norm_str[tok_index] != '\0')
//         tok_index++;
//     tok_index++;
// 
//     int tok_size = tok_index - *str_index;
//     char* token = malloc(tok_size);
// 
//     for (int i = 0; i < tok_size; i++)
//         token[i] = norm_str[*str_index+i];
//     token[tok_size-1] = '\0';
//     *str_index += tok_size;
// 
//     int reg = -1;
//     for (int i = 0; i < sizeof(reg_defs)/sizeof(reg_defs[i]); i++)
//     {   
//         if (strcmp(token, reg_defs[i].str) == 0)
//             reg = i;
//     }
//     free(token);
// 
//     if (reg == -1)
//         throw_error("Unknown register");
// 
//     return reg;
// }
// 
// const char* extract_token(const char* norm_str, int* str_index)
// {
//     // We can probably use this in all of the other 
//     // parse functions, in revision two.
// 
//     if (norm_str[*str_index] == ',' || norm_str[*str_index] == '\0')
//         throw_error("Empty Operand");
// 
//     int tok_index = *str_index;
//     while (norm_str[tok_index] != ',' && norm_str[tok_index] != '\0')
//         tok_index++;
// 
//     int tok_size = tok_index - *str_index + 1;
//     char* token = malloc(tok_size);
// 
//     for (int i = 0; i < tok_size; i++)
//         token[i] = norm_str[*str_index+i];
//     token[tok_size-1] = '\0';
//     *str_index += tok_size;
// 
//     return token;
// }
// 
// int parse_int(const char* num)
// {
//     int num_size = 0;
//     int index = 0;
//     while (num[num_size++]);
//     num_size--;
//     
//     int power = 1;
//     int64_t value = 0;
//     bool negative = false;
// 
//     if (num[index] == '-')
//     {
//         negative = true;
//         index++;
//     }
// 
//     if (num[index] == '0')
//     {
//         index++;
//         switch (num[index++]) 
//         {
//             case 'b':
//                 // Check valid characters
//                 for (int i = index; num[i]; i++)
//                 {
//                     if (num[i] < '0' || num[i] > '1')
//                         throw_error("Unknown Immediate Symbol");
//                 }
//                 
//                 for (int i = num_size - 1; i >= index; i--)
//                 {
//                     value += (num[i]-'0') * power;
//                     power *= 2;
//                 }
//                 
//                 return (negative) ? -value : value;
// 
//             case 'x':
//                 // Check valid characters
//                 for (int i = index; num[i]; i++)
//                 {
//                     if ((num[i] < '0' || num[i] > '9') &&
//                         (num[i] < 'a' || num[i] > 'f'))
//                     {
//                         throw_error("Unknown Immediate Symbol");
//                     }
//                 }
//                 
//                 for (int i = num_size - 1; i >= index; i--)
//                 {
//                     int digit;
//                     if (num[i] >= 'a')
//                         digit = num[i] - 'a' + 10;
//                     else
//                         digit = num[i] - '0';   
//                     value += digit * power;
//                     power *= 16;
//                 }
//                 
//                 return (negative) ? -value : value;
//             default:
//                 index -= 2;
//                 // Intentional fallthrough to default integer processing.
//         }
//     }
//     
//     // Check valid characters
//     for (int i = index; num[i]; i++)
//     {
//         if (num[i] < '0' || num[i] > '9')
//             throw_error("Unknown Immediate Symbol");
//     }
// 
//     for (int i = num_size - 1; i >= index; i--)
//     {
//         value += (num[i]-'0') * power;
//         power *= 10;
//     }
// 
//     return (negative) ? -value : value;
// }
// 
// int parse_imm(const char* norm_str, int* str_index)
// {
//     if (norm_str[*str_index] == ',')
//         throw_error("Empty Operand");
// 
//     int tok_index = *str_index;
//     while (norm_str[tok_index] != ',' && norm_str[tok_index] != '\0')
//         tok_index++;
// 
//     int tok_size = tok_index - *str_index + 1;
//     char* token = malloc(tok_size);
// 
//     for (int i = 0; i < tok_size; i++)
//         token[i] = norm_str[*str_index+i];
//     token[tok_size-1] = '\0';
//     *str_index += tok_size - 1;
// 
//     int ret = parse_int(token);
//     free(token);
//     return ret;
// }