#include "parser.h"
#include "error.h"
#include <stdlib.h>
#include "symtbl.h"

#include "stdio.h"

void normalize_line(char* line, char* norm_buf)
{
  // Validate letters
  for (int i = 0; true; i++)
  {
    char c = line[i];

    // Alphabetic
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
      continue;

    // Numeric
    if ('0' <= c && c <= '9')
      continue;
  
    // Special
    switch (c)
    {
      case ',':
      case '_':
      case '.':
      case ' ':
      case ':':
        continue;

      // End of Line
      case '\0':
      case '\n':
      case ';':
        line[i] = '\0';
        goto end_char_check;
    }
  
    // Unknown char, invalid.
    throw_error("Invalid character \'%c\'", c);
  }
  end_char_check:

  int line_pos = 0;
  int norm_pos = 0;

  // Check for label count
  int label_count = 0;
  for (int i = 0; line[i]; i++)
  {
    if (line[i] == ':')
      label_count++;
  }
  if (label_count > 1)
    throw_error("Too many labels on one line");

  // Skip pre-letter whitespace
  while (line[line_pos] && line[line_pos] == ' ')
    line_pos++;
  if (!line[line_pos])
    goto str_finish;

  // Label processing
  if (label_count)
  {
    if ('0' <= line[line_pos] && line[line_pos] <= '9')
      throw_error("Label cannot begin with a number");

    bool whitespace = false;
    while(line[line_pos] != ':')
    {
      char c = line[line_pos++];
      if (('a' <= c && c <= 'z') || ('0' <= c && c <= '9') || 
        (c == '.') || (c == '_'))
      {
        if (whitespace && norm_buf[norm_pos-1] != ',')
          throw_error("Split token");
        norm_buf[norm_pos++] = c;
      }
      else if ('A' <= c && c <= 'Z')
      {
        if (whitespace && norm_buf[norm_pos-1] != ',')
          throw_error("Split token");
        norm_buf[norm_pos++] = c + 32;
      }
      else if (c == ' ')
      {
        whitespace = true;
        continue;
      }
      else if (c == '\0')
        break;
      else
        throw_error("Invalid label character \'%c\'", c);

      whitespace = false;
    }
    line_pos++;
    norm_buf[norm_pos++] = ':';

    // Skip post-label whitespace
    while (line[line_pos] && line[line_pos] == ' ')
      line_pos++;
    if (!line[line_pos])
      goto str_finish;
  }

  // Mnemonic processing
  while(true)
  {
    char c = line[line_pos++];
    if (('a' <= c && c <= 'z') || c == '.')
      norm_buf[norm_pos++] = c;
    else if ('A' <= c && c <= 'Z')
      norm_buf[norm_pos++] = c + 32;
    else if (c == ' ' || c == '\0')
      break;
    else
      throw_error("Invalid mnemonic character \'%c\'", c);
  }
  if (!line[line_pos-1])
    goto str_finish;
  norm_buf[norm_pos++] = ' ';

  // Skip post-mnemonic whitespace
  while (line[line_pos] && line[line_pos] == ' ')
    line_pos++;
  if (!line[line_pos])
    goto str_finish;

  // Operand processing
  bool whitespace = false;
  while (true)
  {
    char c = line[line_pos++];
    if (('a' <= c && c <= 'z') || ('0' <= c && c <= '9') || 
        (c == '.') || (c == '_'))
    {
      if (whitespace && norm_buf[norm_pos-1] != ',')
        throw_error("Split token");
      norm_buf[norm_pos++] = c;
    }
    else if ('A' <= c && c <= 'Z')
    {
      if (whitespace && norm_buf[norm_pos-1] != ',')
        throw_error("Split token");
      norm_buf[norm_pos++] = c + 32;
    }
    else if (c == ',')
    {
      if (norm_buf[norm_pos-1] == ',')
        throw_error("Empty operand");
      norm_buf[norm_pos++] = c;
    }
    else if (c == '\0')
    {
      if (norm_buf[norm_pos-1] == ',')
        throw_error("Excess operand");
      norm_buf[norm_pos++] = c;
      break;
    }
    else if (c == ' ')
    {
      whitespace = true;
      continue;
    }
    else
      throw_error("Invalid operand character \'%c\'", c);

    whitespace = false;
  }

  str_finish:
  norm_buf[norm_pos] = '\0';
  return;
}

int parse_line(char* line, int byte_offset, parser_ctx* ctx)
{
  int line_pos = 0;
  char norm_line[100];
  char temp_buf[100];

  // Normalize line
  normalize_line(line, norm_line);

  // Detect and parse label
  for (int i = 0; line[i]; i++)
  {
    if (line[i] == ':')
    {
      int j;
      for (j = 0; line[j] != ':'; j++)
        temp_buf[j] = line[j];
      temp_buf[j] = '\0';
      allocate_symbol(temp_buf, byte_offset, ctx->symtbl);
      line_pos = i+1;
      break;
    }
  }

  

  // Break early if EOL
  if (!line[line_pos])
    return 0;

  // Detect and parse mnemonic
  for (int i = 0; line[line_pos] != ' '; i++)
    temp_buf[i] = line[line_pos++];
  temp_buf[line_pos++] = '\0';



  //! Break early if EOL
  if (!line[line_pos])
    return -1;
  

  //* Normalize line
  //^ Detect and allocate label
  //^ Detect mnemonic
    //! Mnemonic must return a byte offset that we return.
  //^ Send rest of string (operands) to callback
  //^ Design primary callback functions
  //^ Design parse helper functions
}



// #include "parser.h"
// #include "error.h"
// #include <stdlib.h>
// #include <string.h>
// #include <stdio.h>
// 
// // Parse a single (null-terminated) string
// // Termination chars: '\0', '\n', ';'
// 
// bool valid_char(char c)
// {
//     // Numeric
//     if ('0' <= c && c <= '9')
//         return true;
// 
//     // Alphabetic
//     if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
//         return true;
// 
//     // Special
//     switch (c) 
//     {
//         // Special
//         case ',':
//         case '-':
//         case '_':
//         case '.':
//         case ';':
//         case ':':
//             return true;
// 
//         // Whitespace
//         case  ' ':
//         case '\n':
//         case '\0':
//             return true;
//     }
// 
//     // Unknown char, invalid.
//     return false;
// }
// 
// char* normalize_line(const char* str)
// {
//     int str_size = 0;
//     while (str[str_size])
//     {
//         if (str[str_size] == ';')
//         {
//             str_size++;
//             break;
//         }
//         else if (!valid_char(str[str_size]))
//         {
//             throw_error("Invalid Char");
//         }
// 
//         str_size++;
//     }
// 
//     int norm_index = 0;
//     char* norm_str = malloc(str_size);
// 
//     // const char* str constraints:
//         // Must be null-terminated
//         // Must contain a mnemonic (at least one non-whitespace character)
//             // Mnemonic can be invalid
// 
// 
//     int i = 0;
// 
//     // Skip whitespace before mnemonic
//     bool colon = false;
//     whitespace:
//     while(str[i] == ' ')
//         i++;
//     if (str[i] == ':')
//     {
//         i++;
//         if (colon)
//             throw_error("Two labels");
//         colon = true;
//         goto whitespace;
//     }
// 
//     // Result: norm_str = "Mnemonic "
//     while(str[i] != ' ' && str[i] != '\n' && str[i] != '\0' && str[i] != ';')
//         norm_str[norm_index++] = str[i++];
//     norm_str[norm_index++] = ' ';
// 
//     if (str[i] == ';')
//     {
//         norm_str[norm_index++] = '\0';
//         return norm_str;
//     }
// 
//     // Now save to comma, remove all whitespace
//     // Until we reach \n or \0
//     //? Form: mnemonic op1,op2,op3
//     //! Test for split tokens "op 1," vs "op1,"
// 
//     enum 
//     {
//         not_parsed,
//         parsing,
//         parsed
//     } token_parsed = not_parsed;
//     while (str[i] != '\n' && str[i] != '\0')
//     {
//         // Remove comments
//         if (str[i] == ';')
//         {
//             norm_str[norm_index++] = '\0';
//             break;
//         }
// 
//         // Reset token when encountering ','
//         if (str[i] == ',')
//         {
//             token_parsed = not_parsed;
//             norm_str[norm_index++] = str[i++];
//             continue;
//         }
//         // Skip whitespace
//         else if (str[i] == ' ')
//         {
//             if (token_parsed == parsing)
//                 token_parsed = parsed;
//             i++;
//             continue;
//         }
// 
//         // Prevent split tokens "op 1," vs "op1,"
//         if (token_parsed == parsed)
//         {
//             throw_error("Split token");
//         }
// 
//         token_parsed = parsing;
//         norm_str[norm_index++] = str[i++];
//     }
//     norm_str[norm_index] = '\0';
// 
//     // Decapitalize
//     for (int i = 0; norm_str[i]; i++)
//     {
//         if ('A' <= norm_str[i] && norm_str[i] <= 'Z')
//             norm_str[i] += 32;
//     }
// 
//     return norm_str;
// }
// 
// mnemonic_index parse_mnemonic(const char* norm_str, int* str_index)
// {
//     int token_size = 0;
//     while (norm_str[token_size++] != ' ');
// 
//     char* mnemonic = malloc(token_size);
//     for (int i = 0; i < token_size-1; i++)
//         mnemonic[i] = norm_str[i];
//     mnemonic[token_size-1] = '\0';
// 
//     mnemonic_index parsed_mnemonic = -1;
//     for (int i = 0; i < sizeof(instr_defs)/sizeof(instr_defs[i]); i++)
//     {   
//         if (strcmp(mnemonic, instr_defs[i].str) == 0)
//             parsed_mnemonic = i;
//     }
//     *str_index += token_size;
//     free(mnemonic);
// 
// 
//     return parsed_mnemonic;
// }
// 
// int parse_reg(const char* norm_str, int* str_index)
// {
//     if (norm_str[*str_index] == ',')
//         throw_error("Empty Operand");
//     
//     int tok_index = *str_index;
//     while (norm_str[tok_index] != ',' && norm_str[tok_index] != '\0')
//         tok_index++;
//     tok_index++;
// 
//     int tok_size = tok_index - *str_index;
//     char* token = malloc(tok_size);
// 
//     for (int i = 0; i < tok_size; i++)
//         token[i] = norm_str[*str_index+i];
//     token[tok_size-1] = '\0';
//     *str_index += tok_size;
// 
//     int reg = -1;
//     for (int i = 0; i < sizeof(reg_defs)/sizeof(reg_defs[i]); i++)
//     {   
//         if (strcmp(token, reg_defs[i].str) == 0)
//             reg = i;
//     }
//     free(token);
// 
//     if (reg == -1)
//         throw_error("Unknown register");
// 
//     return reg;
// }
// 
// const char* extract_token(const char* norm_str, int* str_index)
// {
//     // We can probably use this in all of the other 
//     // parse functions, in revision two.
// 
//     if (norm_str[*str_index] == ',' || norm_str[*str_index] == '\0')
//         throw_error("Empty Operand");
// 
//     int tok_index = *str_index;
//     while (norm_str[tok_index] != ',' && norm_str[tok_index] != '\0')
//         tok_index++;
// 
//     int tok_size = tok_index - *str_index + 1;
//     char* token = malloc(tok_size);
// 
//     for (int i = 0; i < tok_size; i++)
//         token[i] = norm_str[*str_index+i];
//     token[tok_size-1] = '\0';
//     *str_index += tok_size;
// 
//     return token;
// }
// 
// int parse_int(const char* num)
// {
//     int num_size = 0;
//     int index = 0;
//     while (num[num_size++]);
//     num_size--;
//     
//     int power = 1;
//     int64_t value = 0;
//     bool negative = false;
// 
//     if (num[index] == '-')
//     {
//         negative = true;
//         index++;
//     }
// 
//     if (num[index] == '0')
//     {
//         index++;
//         switch (num[index++]) 
//         {
//             case 'b':
//                 // Check valid characters
//                 for (int i = index; num[i]; i++)
//                 {
//                     if (num[i] < '0' || num[i] > '1')
//                         throw_error("Unknown Immediate Symbol");
//                 }
//                 
//                 for (int i = num_size - 1; i >= index; i--)
//                 {
//                     value += (num[i]-'0') * power;
//                     power *= 2;
//                 }
//                 
//                 return (negative) ? -value : value;
// 
//             case 'x':
//                 // Check valid characters
//                 for (int i = index; num[i]; i++)
//                 {
//                     if ((num[i] < '0' || num[i] > '9') &&
//                         (num[i] < 'a' || num[i] > 'f'))
//                     {
//                         throw_error("Unknown Immediate Symbol");
//                     }
//                 }
//                 
//                 for (int i = num_size - 1; i >= index; i--)
//                 {
//                     int digit;
//                     if (num[i] >= 'a')
//                         digit = num[i] - 'a' + 10;
//                     else
//                         digit = num[i] - '0';   
//                     value += digit * power;
//                     power *= 16;
//                 }
//                 
//                 return (negative) ? -value : value;
//             default:
//                 index -= 2;
//                 // Intentional fallthrough to default integer processing.
//         }
//     }
//     
//     // Check valid characters
//     for (int i = index; num[i]; i++)
//     {
//         if (num[i] < '0' || num[i] > '9')
//             throw_error("Unknown Immediate Symbol");
//     }
// 
//     for (int i = num_size - 1; i >= index; i--)
//     {
//         value += (num[i]-'0') * power;
//         power *= 10;
//     }
// 
//     return (negative) ? -value : value;
// }
// 
// int parse_imm(const char* norm_str, int* str_index)
// {
//     if (norm_str[*str_index] == ',')
//         throw_error("Empty Operand");
// 
//     int tok_index = *str_index;
//     while (norm_str[tok_index] != ',' && norm_str[tok_index] != '\0')
//         tok_index++;
// 
//     int tok_size = tok_index - *str_index + 1;
//     char* token = malloc(tok_size);
// 
//     for (int i = 0; i < tok_size; i++)
//         token[i] = norm_str[*str_index+i];
//     token[tok_size-1] = '\0';
//     *str_index += tok_size - 1;
// 
//     int ret = parse_int(token);
//     free(token);
//     return ret;
// }
// 
// void parse_NO_TYPE(const char* norm_str, int* str_index, instr_t* parsed_instr)
// {
//     switch (parsed_instr->mnemonic) 
//     {
//     case FENCE:
//         const char* op[2];
//         op[0] = extract_token(norm_str, str_index);
//         op[1] = extract_token(norm_str, str_index);
//         parsed_instr->op[0].type = SPECIAL;
//         parsed_instr->op[1].type = SPECIAL;
//         parsed_instr->op[2].type = -1;
// 
//         for (int i = 0; i < 2; i++)
//         {
//             int j = 0;
//             while (op[i][j])
//             {
//                 switch (op[i][j++]) 
//                 {
//                 case 'i':
//                     if ((parsed_instr->op[i].val >> 3) & 1)
//                         throw_error("Double symbol in FENCE.");
//                     parsed_instr->op[i].val |= 1 << 3;
//                     break;
// 
//                 case 'o':
//                     if ((parsed_instr->op[i].val >> 2) & 1)
//                         throw_error("Double symbol in FENCE.");
//                     parsed_instr->op[i].val |= 1 << 2;
//                     break;
// 
//                 case 'r':
//                     if ((parsed_instr->op[i].val >> 1) & 1)
//                         throw_error("Double symbol in FENCE.");
//                     parsed_instr->op[i].val |= 1 << 1;
//                     break;
// 
//                 case 'w':
//                     if ((parsed_instr->op[i].val >> 0) & 1)
//                         throw_error("Double symbol in FENCE.");
//                     parsed_instr->op[i].val |= 1 << 0;
//                     break;
//                     
//                 default:
//                     throw_error("Unknow symbol in FENCE (iorw).");
//                     break;
//                 }
//             }
// 
//             if (parsed_instr->op[i].val == 0)
//                 throw_error("Empty Operand.");
//         }
//         break;
// 
//     case ECALL:
//     case EBREAK:
//         parsed_instr->op[0].type = -1;
//         parsed_instr->op[1].type = -1;
//         parsed_instr->op[2].type = -1;
//         parsed_instr->op[0].val = -1;
//         parsed_instr->op[1].val = -1;
//         parsed_instr->op[2].val = -1;
// 
//         if (norm_str[*str_index] != '\0')
//             throw_error("Excess operand");
//         return;
//     
//     default:
//         throw_error("Unknown \"NO_TYPE\" mnemonic.");
//         return;
//     }
// 
//     return;
// }
// 
// instr_t parse_instruction(const char* str)
// {
//     // Expected form:
//     // Mnemonic op1, op2, op3
//     // Capitalization not checked
//     // ; is comment (ignore all letters after)
//     // Line MUST have a mnemonic to be properly parsed
//         // Whitespace only lines are expected to be skipped prior
// 
//     instr_t parsed_instr = {.mnemonic = ERROR_MNEMONIC, .type = -1};
//     
//     for (int i = 0; true; i++)
//     {
//         // Return an invalid/empty instr_t if this line is whitespace only
//         if (str[i] == '\0' || str[i] == ';' || str[i] == '\n')
//             return parsed_instr;
//         else if (str[i] == ' ')
//             continue;
// 
//         break;
//     }
// 
// 
//     char* norm_str = normalize_line(str);
// 
// 
//     int str_index = 0;
//     parsed_instr.mnemonic = parse_mnemonic(norm_str, &str_index);
//     if (parsed_instr.mnemonic == ERROR_MNEMONIC)
//     {
//         parsed_instr.type = ERROR_TYPE;
//         goto parse_switch_mnemonic;
//     }
// 
//     parsed_instr.type = instr_defs[parsed_instr.mnemonic].type;
// 
//     // Switch statement here for .type
//     // Then we can run an appropriate number of parse_operand
//     // Test for excess operands: throw error
// 
//     
//     parse_switch_mnemonic:
//     switch(parsed_instr.type)
//     {
//         // rd, rs1, rs2
//         case R_TYPE:
//             parsed_instr.op[0].type = REGISTER;
//             parsed_instr.op[0].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[1].type = REGISTER;
//             parsed_instr.op[1].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[2].type = REGISTER;
//             parsed_instr.op[2].val = parse_reg(norm_str, &str_index);
// 
//             break;
// 
//         // rd, rs1, imm
//         case I_TYPE:
//             parsed_instr.op[0].type = REGISTER;
//             parsed_instr.op[0].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[1].type = REGISTER;
//             parsed_instr.op[1].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[2].type = IMMEDIATE;
//             parsed_instr.op[2].val = parse_imm(norm_str, &str_index);
// 
//             break;
// 
//         // rs1, rs2, imm
//         case S_TYPE:
//             parsed_instr.op[0].type = REGISTER;
//             parsed_instr.op[0].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[1].type = REGISTER;
//             parsed_instr.op[1].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[2].type = IMMEDIATE;
//             parsed_instr.op[2].val = parse_imm(norm_str, &str_index);
// 
//             break;
// 
//         // rs1, rs2, imm
//         case B_TYPE:
//             parsed_instr.op[0].type = REGISTER;
//             parsed_instr.op[0].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[1].type = REGISTER;
//             parsed_instr.op[1].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[2].type = IMMEDIATE;
//             parsed_instr.op[2].val = parse_imm(norm_str, &str_index);
// 
//             break;
// 
//         // rd, imm
//         case U_TYPE:
//             parsed_instr.op[0].type = REGISTER;
//             parsed_instr.op[0].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[1].type = IMMEDIATE;
//             parsed_instr.op[1].val = parse_imm(norm_str, &str_index);
// 
//             parsed_instr.op[2].type = -1;
//             parsed_instr.op[2].val = -1;
// 
//             break;
// 
//         // rd, imm
//         case J_TYPE:
//             parsed_instr.op[0].type = REGISTER;
//             parsed_instr.op[0].val = parse_reg(norm_str, &str_index);
// 
//             parsed_instr.op[1].type = IMMEDIATE;
//             parsed_instr.op[1].val = parse_imm(norm_str, &str_index);
// 
//             parsed_instr.op[2].type = -1;
//             parsed_instr.op[2].val = -1;
// 
//             break;
// 
//         // Special Mnemonic
//         case NO_TYPE:
//             parse_NO_TYPE(norm_str, &str_index, &parsed_instr);
//             break;
//         
//         // Unknown / Error
//         case ERROR_TYPE:
//             throw_error("Unknown Mnemonic");
//             break;
//     }
// 
//     if (norm_str[str_index-1] == ',')
//         throw_error("Excess Operand");
// 
//     free(norm_str);
// 
//     return parsed_instr;
// }
// 

