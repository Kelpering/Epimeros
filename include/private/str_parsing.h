#ifndef __EPIMEROS_PRIVATE_PARSING_H__
#define __EPIMEROS_PRIVATE_PARSING_H__
#include <stdint.h>
#include "private/parser.h"

// Treats '\n' as '\0', simplifies conditionals
char skip_whitespace(const char* str, int* index);
// Treats '\n' as '\0', simplifies conditionals
char skip_until(char delim, const char* str, int* index);
char* extract_delim(char delim, const char* str, int* index);
char* extract_token(const char* str, int* index);
char* extract_operand(const char* str, int* index);
char* extract_operand_strict(const char* str, int* index);
int64_t parse_int64_t(char* str);
uint64_t parse_uint64_t(char* str);
uint32_t parse_reg(char* str, parser_ctx* ctx);
int64_t parse_imm(char* imm, parser_ctx* ctx);
int64_t parse_sysmacro(char* str, parser_ctx* ctx);
uint32_t process_bit_range(int64_t value, int bitsize, bool sign);

//^ extract_param vs extract_operand
//^ Operand is termchar = ','; skip (parenthesis enclosed params)
//^ param is termchar = ',' & ')'; Should only be used in functions like %hi()
//^ Maybe an "offset(reg)" extractor too?
//^ Could work if we counted params
//^ It would also be MUCH easier if we just defaulted to using it.

#endif // __EPIMEROS_PRIVATE_PARSING_H__