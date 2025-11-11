#ifndef __EPIMEROS_PRIVATE_PREPROCESSOR_H__
#define __EPIMEROS_PRIVATE_PREPROCESSOR_H__
#include <stdio.h>

typedef struct macro_t
{
  char* macro;
  char* str_replace;
  int param_count;
  struct macro_t* next;
} macro_t;
macro_t* search_macro(const char* macro, macro_t* head);
macro_t* alloc_macro(
  const char* macro, 
  const char* str_replace, 
  int param_count, 
  macro_t* head
);

FILE* preprocess_file(const char* path, const char* path_processed);

#endif // __EPIMEROS_PRIVATE_PREPROCESSOR_H__