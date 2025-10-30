#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "error.h"


void parse_file(FILE* file, parser_ctx* ctx)
{
  line_num = 0;
  int byte_offset = 0;
  char line_buf[100];

  instr_t* head = malloc(sizeof(instr_t));
  head->def = NULL;
  head->next = NULL;
  head->op = NULL;
  head->op_size = 0;
  instr_t* next_node = head;

  rewind(file);
  while(fgets(line_buf, sizeof(line_buf), file))
  {
    line_num++;

    // Detect line buffer overflow
    int i = 0;
    while (line_buf[i++]);
    if (line_buf[i-2] != '\n' && !feof(file))
      throw_error("Line buffer overflow");

    // [label:][mnemonic][ops...]{;Comments / Newlines are replaced with '\0'}
    
    //? Attach another node for each instruction (skip empty lines).
    instr_t* temp = parse_line(line_buf, byte_offset, ctx);
    if (temp)
    {
      next_node->next = temp;
      next_node = next_node->next;
    }
  }
  instr_t* curr_instr = head->next;
  
  while(curr_instr)
  {
    uint32_t hex = curr_instr->def->trans_cb(curr_instr, ctx);
    printf("HEXCODE: 0x%.8x\n", hex);

    curr_instr = curr_instr->next;
  }

  // Free
  instr_t* tmp = head;
  while(tmp)
  {
    instr_t* t = tmp->next;
    free(tmp->op);
    free(tmp);
    tmp = t;
  }

  return;
}
// void parse_file(FILE* asm_file)
// {
//   line_num = 0;
//   char line_buf[100];
//   rewind(asm_file);
// 
//   while (fgets(line_buf, sizeof(line_buf), asm_file))
//   {
//     line_num++;
//     instr_t instr = parse_instruction(line_buf);
//     if (instr.mnemonic == ERROR_MNEMONIC)
//       continue;   // Skip empty lines
// 
// 
//     uint32_t hex = translate_instr(instr);
//     printf("Line %d: 0x%x\n", line_num, hex);
//   }
// }