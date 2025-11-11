#include "sysmacro.h"
#include "error.h"

int64_t macro_hi(int64_t label, parser_ctx* ctx)
{
  uint64_t offset = ctx->starting_offset + ctx->symtbl->sym[label].byte_offset;
  offset >>= 12;

  return offset;
}

int64_t macro_lo(int64_t label, parser_ctx* ctx)
{
  uint64_t offset = ctx->starting_offset + ctx->symtbl->sym[label].byte_offset;
  offset &= 0xFFF;

  return offset;
}

int64_t macro_pcrel_hi(int64_t label, parser_ctx* ctx)
{
  uint64_t offset = ctx->starting_offset + ctx->symtbl->sym[label].byte_offset;
  throw_error("Reference PC in func call");

  return 5;
}

int64_t macro_pcrel_lo(int64_t label, parser_ctx* ctx)
{
  uint64_t offset = ctx->starting_offset + ctx->symtbl->sym[label].byte_offset;
  throw_error("Reference PC in func call");

  return 5;
}