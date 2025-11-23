#include "private/translation.h"

void temp_trans(instr_t* instr_list, parser_ctx* ctx)
{
  while(instr_list->next)
  {
    instr_t* curr = instr_list;
    printf("temp: 0x%x\n", curr->instr_def->trans_cb(curr, ctx));
    
    instr_t* temp = instr_list->next;
    instr_list = temp;
  }
  printf("temp: 0x%x\n", instr_list->instr_def->trans_cb(instr_list, ctx));
}