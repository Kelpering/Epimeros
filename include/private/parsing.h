#ifndef __EPIMEROS_PRIVATE_PARSING_H__
#define __EPIMEROS_PRIVATE_PARSING_H__
#include <stdint.h>

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

// int8_t numeral_def[] = {}
// numeral_def['a'] == 10 // Valid, integer 10
// numeral_def[')'] == -1 // Invalid, -1
// parse_base_int (selected base is input, number string is defined, checks chars)
// parse_int (any number, detected by 0b, 0x, 0o, etc)

#endif // __EPIMEROS_PRIVATE_PARSING_H__