#include <stdio.h>
#include "file.h"
#include "error.h"


void parse_file(FILE* file, parser_ctx* ctx)
{
  line_num = 0;
  int byte_offset = 0;
  char line_buf[100];

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
    byte_offset += parse_line(line_buf, byte_offset, ctx);

    
  }
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