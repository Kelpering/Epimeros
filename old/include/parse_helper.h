#ifndef __EPIMEROS_PARSE_HELPER_H__
#define __EPIMEROS_PARSE_HELPER_H__
#include "parser.h"

op_t parse_reg(const char* line, int* str_index, parser_ctx* ctx);
op_t parse_imm(const char* line, int* str_index, parser_ctx* ctx);

#endif // __EPIMEROS_PARSE_HELPER_H__
