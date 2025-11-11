#ifndef __EPIMEROS_SYSMACRO_H__
#define __EPIMEROS_SYSMACRO_H__
#include <stdint.h>
#include "parser.h"

int64_t macro_hi(int64_t label, parser_ctx* ctx);
int64_t macro_lo(int64_t label, parser_ctx* ctx);
int64_t macro_pcrel_hi(int64_t label, parser_ctx* ctx);
int64_t macro_pcrel_lo(int64_t label, parser_ctx* ctx);

#endif // __EPIMEROS_SYSMACRO_H__