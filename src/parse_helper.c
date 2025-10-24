#include "parse_helper.h"
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
      ctx->instr_list[instr_ctr].callback = set[j].callback;

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
        throw_error("Duplicate register definition \"%s\"", 
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