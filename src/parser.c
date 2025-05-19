#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "symtbl.h"

static int line_num = 1;

static void normalize_line(char* line)
{

    int line_size = 0;
    while (line[line_size++] && line_size < MAX_LINE_SIZE);

    if (line_size >= MAX_LINE_SIZE)
    {
        printf("Error: Max line size exceeded. (Line: %d)\n", line_num);
        exit(1);
    }

    // Add newline if absent
    if (line[line_size-2] != '\n')
    {
        printf("Warn: Newline Absent (Line: %d)", line_num);
        line[line_size-2] = '\n';
    }

    for (int i = 0; i < MAX_LINE_SIZE; i++)
    {
        if (line[i] == '\0')
            break;

        // Decapitalize
        if (line[i] >= 'A' && line[i] <= 'Z')
        {
            line[i] += 32;
            continue;
        }

        // Remove line comments
        if (line[i] == '#')
        {
            // Line i+1 will not overflow due to previous check.
            line[i] = '\n';
            line[i+1] = '\0';
            break;
        }
    }

    return;
}

static bool skip_until(char* line, char c, int* pos)
{
    while (line[*pos] != c && line[*pos] != '\n')
        (*pos)++;

    // Maybe return here for info on if we found char

    return (line[*pos] == c);
}

static mnemonic_index detect_mnemonic(char* line, int* const pos)
{
    // Skip past labels
    for (int i = 0; i < MAX_LINE_SIZE; i++)
    {
        if (line[i] == '\n' || line[i] == '\0')
            break;

        if (line[i] == ':')
            *pos = i+1;
    }

    // Skip whitespace
    while (line[*pos] == ' ')
        (*pos)++;

    // End of Line, return.
    if(line[*pos] == '\n' || line[*pos] == '\0')
        return EOF;

    int start_pos = *pos;
    skip_until(line, ' ', pos);

    char* mnemonic = malloc(*pos - start_pos + 1);
    for (int i = 0; i < *pos - start_pos; i++)
        mnemonic[i] = line[start_pos + i];
    mnemonic[*pos - start_pos] = '\0';

    (*pos)++;

    for (int i = 0; i < TOTAL_MNEMONICS; i++)
    {
        if (strcmp(mnemonic, instr_defs[i].str) == 0)
        {
            free(mnemonic);
            return i;
        }
    }

    printf("Error: Unknown mnemonic \"%s\" (Line: %d)\n", mnemonic, line_num);

    exit(1);
}

op_t detect_reg(char* line, int* pos)
{
    op_t op;
    op.type = REGISTER;
    
    // Skip whitespace
    while (line[*pos] == ' ')
        (*pos)++;

    // End of Line, error.
    if(line[*pos] == '\n' || line[*pos] == '\0')
    {
        printf("Error: Expected register (Line: %d)\n", line_num);
        exit(1);
    }

    int start_pos = *pos;
    skip_until(line, ',', pos);
    //! The skip here seems to work even for the last register. Test when finished.
    
    char* reg = malloc(*pos - start_pos + 1);
    for (int i = 0; i < *pos - start_pos; i++)
        reg[i] = line[start_pos + i];
    reg[*pos - start_pos] = '\0';

    (*pos)++;

    // Identify register
    for (int i = 0; i < TOTAL_REGS; i++)
    {
        if (strcmp(reg, reg_str_arr[i].str) == 0)
        {
            op.val = reg_str_arr[i].reg_index;
            printf("Reg: %d\n", op.val);
            free(reg);
            return op;
        }
    }
    printf("Error: Invalid register \"%s\" (Line: %d)\n", reg, line_num);
    exit(1);
}

op_t detect_imm(char* line, int* pos)
{
    exit(1);
}

instr_t* parse_file(FILE* file, int instr_cnt)
{
    instr_t* tokens = malloc(instr_cnt * sizeof(instr_t));
    
    // Start from file beginning
    rewind(file);

    char line[MAX_LINE_SIZE+1];
    int cur = 0;
    while (fgets(line, MAX_LINE_SIZE, file))
    {
        int pos = 0;

        normalize_line(line);

        mnemonic_index mindex = detect_mnemonic(line, &pos);

        if (mindex == EOF)
            goto skip_instr;

        tokens[cur].mnemonic = mindex;
        tokens[cur].type = instr_defs[mindex].type;

        switch (tokens[cur].type)
        {
            case R_TYPE:
                tokens[cur].op[0] = detect_reg(line, &pos);
                tokens[cur].op[1] = detect_reg(line, &pos);
                tokens[cur].op[2] = detect_reg(line, &pos);
                if (!skip_until(line, '\n', &pos))
                {
                    printf("Error: Too many operands (Line: %d)\n", line_num);
                    exit(1);
                }
                break;
            
            case I_TYPE:
            case S_TYPE:
            case B_TYPE:
                tokens[cur].op[0] = detect_reg(line, &pos);
                tokens[cur].op[1] = detect_reg(line, &pos);
                tokens[cur].op[2] = detect_imm(line, &pos);
                if (!skip_until(line, '\n', &pos))
                {
                    printf("Error: Too many operands (Line: %d)\n", line_num);
                    exit(1);
                }
                break;
            
            case U_TYPE:
            case J_TYPE:
                tokens[cur].op[0] = detect_reg(line, &pos);
                tokens[cur].op[1] = detect_imm(line, &pos);
                if (!skip_until(line, '\n', &pos))
                {
                    printf("Error: Too many operands (Line: %d)\n", line_num);
                    exit(1);
                }
                break;
            
            case NO_TYPE:
                printf("Error: Unhandled mnemonic \"%s\" (Line: %d)\n", instr_defs[mindex].str, line_num);
                exit(1);
            
            
        }

        cur++;

        skip_instr:
        line_num++;
    }


    return tokens;
}
