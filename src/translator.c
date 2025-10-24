// #include "translator.h"
// #include "error.h"
// 
// uint32_t process_imm(int64_t imm, instr_type type)
// {
//     uint32_t ret = 0;
// 
//     switch (type) 
//     {
//         // Should not be called
//         case R_TYPE:
//             throw_error("Register Error Immediate");
//             break;
// 
//         // (2047) - (-2048)
//         case I_TYPE:
//             if ((imm > (1<<11)-1) | (imm < -(1<<11)))
//                 throw_error("Immediate Out of Range");
// 
//             ret |= (((imm & 0b00000000001) >> 0) << 20);
//             ret |= (((imm & 0b00000011110) >> 1) << 21);
//             ret |= (((imm & 0b11111100000) >> 5) << 25);
//             if (imm < 0) 
//                 ret |= 1 << 31;
// 
//             break;
//         
//         // (2047) - (-2048)
//         case S_TYPE:
//             if ((imm > (1<<11)-1) | (imm < -(1<<11)))
//                 throw_error("Immediate Out of Range");
// 
//             ret |= (((imm & 0b00000000001) >> 0) << 7);
//             ret |= (((imm & 0b00000011110) >> 1) << 8);
//             ret |= (((imm & 0b11111100000) >> 5) << 25);
//             if (imm < 0) 
//                 ret |= 1 << 31;
// 
//             break;
// 
//         // (4094) - (-4096), multiple of 2
//         case B_TYPE:
//             if (imm % (1<<1))
//                 throw_error("Immediate Out of Alignment");
//             if ((imm > (1<<12)-1) | (imm < -(1<<12)))
//                 throw_error("Immediate Out Of Range");
// 
//             ret |= (((imm & 0b000000011110) >>  1) <<  8);
//             ret |= (((imm & 0b011111100000) >>  5) << 25);
//             ret |= (((imm & 0b100000000000) >> 11) <<  7);
//             if (imm < 0) 
//                 ret |= 1 << 31;
//             
//             break;
// 
//         // (524287) - (-524288)
//         case U_TYPE:
//             if ((imm > (1L<<19)-1) | (imm < -(1L<<19)))
//                 throw_error("Immediate Out Of Range");
// 
//             ret |= (((imm & 0b00000000000'11111111) >>  0) << 12);
//             ret |= (((imm & 0b11111111111'00000000) >>  8) << 20);
//             if (imm < 0) 
//                 ret |= 1 << 31;
// 
//             break;
// 
//         // (1048574) - (-1048576), multiple of 2
//         case J_TYPE:
//             if (imm % (1<<1))
//                 throw_error("Immediate Out of Alignment");
//             if ((imm > (1L<<20)-1) | (imm < -(1L<<20)))
//                 throw_error("Immediate Out Of Range");
// 
//             ret |= (((imm & 0b00000000000000011110) >>  1) << 21);
//             ret |= (((imm & 0b00000000011111100000) >>  5) << 25);
//             ret |= (((imm & 0b00000000100000000000) >> 11) << 20);
//             ret |= (((imm & 0b11111111000000000000) >> 12) << 12);
//             if (imm < 0) 
//                 ret |= 1 << 31;
// 
//             break;
// 
//         // Special Mnemonic
//         case NO_TYPE:
//             throw_error("Special Mnemonic Immediate");
//             break;
// 
//         // Unknown / Error
//         case ERROR_TYPE:
//             throw_error("Error Mnemonic Immediate");
//             break;
//     }
// 
//     return ret;
// }
// 
// uint32_t translate_NO_TYPE(instr_t* instr)
// {
//     switch (instr->mnemonic) 
//     {
//     case FENCE:
//         uint32_t bytecode = 0;
//         bytecode |= instr->op[0].val << 24;
//         bytecode |= instr->op[1].val << 20;
//         return bytecode;
// 
//     case ECALL:
//     case EBREAK:
//         return 0;
//         break;
//     default:
//         throw_error("Unknown Mnemonic NO_TYPE translate");
//     }
// }
// 
// uint32_t translate_instr(instr_t instr)
// {
//     uint32_t bytecode = instr_defs[instr.mnemonic].opcode;
// 
//     switch (instr.type) 
//     {
//         // rd, rs1, rs2
//         case R_TYPE:
//             bytecode |= instr.op[0].val << 7;
//             bytecode |= instr.op[1].val << 15;
//             bytecode |= instr.op[2].val << 20;
//             break;
// 
//         // rd, rs1, imm
//         case I_TYPE:
//             bytecode |= instr.op[0].val << 7;
//             bytecode |= instr.op[1].val << 15;
//             bytecode |= process_imm(instr.op[2].val, instr.type);
//             break;
//         
//         // rs1, rs2, imm
//         case S_TYPE:
//             bytecode |= instr.op[0].val << 15;
//             bytecode |= instr.op[1].val << 20;
//             bytecode |= process_imm(instr.op[2].val, instr.type);
//             break;
// 
//         // rs1, rs2, imm
//         case B_TYPE:
//             bytecode |= instr.op[0].val << 15;
//             bytecode |= instr.op[1].val << 20;
//             bytecode |= process_imm(instr.op[2].val, instr.type);
//             break;
// 
//         // rd, imm
//         case U_TYPE:
//             bytecode |= instr.op[0].val << 7;
//             bytecode |= process_imm(instr.op[1].val, instr.type);
//             break;
// 
//         // rd, imm
//         case J_TYPE:
//             bytecode |= instr.op[0].val << 7;
//             bytecode |= process_imm(instr.op[1].val, instr.type);
//             break;
// 
//         // Special Mnemonic
//         case NO_TYPE:
//             bytecode |= translate_NO_TYPE(&instr);
//             break;
// 
//         // Unknown / Error
//         case ERROR_TYPE:
//             throw_error("Error Mnemonic Translation");
//             break;
//     }
// 
//     return bytecode;
// }
