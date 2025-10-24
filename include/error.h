#ifndef __EPIMEROS_ERROR_H__
#define __EPIMEROS_ERROR_H__
#include <stdint.h>

extern int line_num;

void throw_error(const char* str, ...) 
__attribute__((__noreturn__));

#endif // __EPIMEROS_ERROR_H__
