#pragma once
#include <regex>
#include <string>
#include <sstream>

#define regex_SECTION regex("^.section\\s*")
#define regex_GLOBAL regex("^.global\\s*")
#define regex_EXTERN regex("^.extern\\s*")
#define regex_WORD regex("^.word\\s*")
#define regex_SKIP regex("^.skip\\s*")
#define regex_EQU regex("^.equ\\s*")
#define regex_END regex("^.end\\s*")
#define regex_LABEL regex("^([A-Za-z_]\\w*):\\s*")
#define regex_COMMENT regex("^#.*\\s*")

#define regex_INSTR regex("^(halt|int|iret|call|ret|jmp|jeq|jne|jgt|push|pop|xchg|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|ldr|str)\\s*")

#define regex_HALT regex("^halt\\s*")
#define regex_INT regex("^int\\s*")
#define regex_IRET regex("^iret\\s*")
#define regex_CALL regex("^call\\s*")
#define regex_RET regex("^ret\\s*")
#define regex_JMP regex("^jmp\\s*")
#define regex_JEQ regex("^jeq\\s*")
#define regex_JNE regex("^jne\\s*")
#define regex_JGT regex("^jgt\\s*")
#define regex_PUSH regex("^push\\s*")
#define regex_POP regex("^pop\\s*")
#define regex_XCHG regex("^xchg\\s*")
#define regex_ADD regex("^add\\s*")
#define regex_SUB regex("^sub\\s*")
#define regex_MUL regex("^mul\\s*")
#define regex_DIV regex("^div\\s*")
#define regex_CMP regex("^cmp\\s*")
#define regex_NOT regex("^not\\s*")
#define regex_AND regex("^and\\s*")
#define regex_OR regex("^or\\s*")
#define regex_XOR regex("^xor\\s*")
#define regex_TEST regex("^test\\s*")
#define regex_SHL regex("^shl\\s*")
#define regex_SHR regex("^shr\\s*")
#define regex_LDR regex("^ldr\\s*")
#define regex_STR regex("^str\\s*")

#define regex_DEC_NUM regex("^(\\-{0,1})[0-9]+\\s*")
#define regex_HEX_NUM regex("^(0[xX])([0-9A-Fa-f]+)\\s*")
#define regex_hex_part regex("^[0-9A-Fa-f]+\\s*")

#define regex_imm_lit string("^\\$((0[xX][a-fA-F0-9]+)|(0)|([1-9]([0-9]*)))\\s*")//0
#define regex_imm_sym string("^\\$([a-zA-Z_])\\w*\\s*")//0
#define regex_reg_dir string("^(r[0-7])|sp|psw|pc\\s*")//1
#define regex_reg_ind string("^\\[((r[0-7])|sp|psw|pc)\\]\\s*")//2
#define regex_pc_rel string("^%([a-zA-Z_]\\w*)\\s*")//3
#define regex_indirect_sum_lit string("^\\[((r[0-7])|sp|psw|pc)\\+((0[xX][a-fA-F0-9]+)|0|([1-9]([0-9]*)))\\]\\s*")//3
#define regex_indirect_sum_sym string("^\\[((r[0-7])|sp|psw|pc)\\+([a-zA-Z_]\\w*)\\]\\s*")//3
#define regex_mem_lit string("^((0[xX][a-fA-F0-9]+)|(0)|([1-9]([0-9]*)))\\s*")//4
#define regex_mem_sym string("^([a-zA-Z_])\\w*\\s*")//4

#define regex_imm_lit_jmp string("^((0[xX][a-fA-F0-9]+)|0|([1-9]([0-9]*)))\\s*")
#define regex_imm_sym_jmp string("^([a-zA-Z_])\\w*\\s*")
#define regex_reg_dir_jmp string("^\\*((r[0-7])|sp|psw|pc)\\s*")
#define regex_reg_ind_jmp string("^\\*(\\[((r[0-7])|sp|psw|pc)\\])\\s*")
//#define regex_reg_dir_jmp string("^\\*(r[0-7]|sp|pc|psw)\\s*$")
//#define regex_reg_ind_jmp string("^\\*\\[\\s*(r[0-7]|sp|pc|psw)\\s*\\]\\s*$")
#define regex_pc_imm_jmp string("^%([a-zA-Z_]\\w*)\\s*")
#define regex_indirect_sum_lit_jmp string("^\\*(\\[(((r[0-7])|sp|psw|pc)\\+((0[xX][a-fA-F0-9]+)|0|([1-9]([0-9]*))))\\])\\s*")
#define regex_indirect_sum_sym_jmp string("^\\*(\\[(((r[0-7])|sp|psw|pc)\\+([a-zA-Z_])\\w*)\\])\\s*")
//#define regex_indirect_sum_lit_jmp string("^\\*\\[\\s*(r[0-7]|sp|pc|psw)\\s*\\+\\s*(0|-?[1-9]\\d*|0[xX][a-fA-F0-9]+)\\s*\\]\\s*$")
//#define regex_indirect_sum_sym_jmp string("^\\*\\[\\s*(r[0-7]|sp|pc|psw)\\s*\\+\\s*([A-Za-z_]\\w*)\\s*\\]\\s*$")
#define regex_mem_lit_jmp string("^\\*((0[xX][a-fA-F0-9]+)|0|([1-9]([0-9]*)))\\s*")
//#define regex_mem_lit_jmp string("^\\*(0|-?[1-9]\\d*|0[xX][a-fA-F0-9]+)\\s*$")
#define regex_mem_sym_jmp string("^\\*([a-zA-Z_])\\w*\\s*")
//#define regex_mem_sym_jmp string("^\\*([A-Za-z_]\\w*)\\s*$")

#define regex_indirect_sum_offset regex("^((0[xX][a-fA-F0-9]+)|0|([1-9]([0-9]*)))\\s*")
#define regex_indirect_sum_reg regex("^((r[0-7])|sp|psw|pc)\\s*")
#define regex_indirect_sum_symbol regex("^(([a-zA-Z_])\\w*)\\s*")

string addChar = "|";
string data3B = regex_reg_dir + addChar + regex_reg_ind;
string data5B = regex_imm_lit + addChar + regex_imm_sym + addChar + regex_pc_rel + addChar + regex_indirect_sum_lit + addChar + regex_indirect_sum_sym + addChar + regex_mem_lit + addChar + regex_mem_sym;
#define regex_3B_data regex(data3B)
#define regex_5B_data regex(data5B)

string jump3B = regex_reg_dir_jmp + addChar + regex_reg_ind_jmp;
string jump5B = regex_imm_lit_jmp + addChar + regex_imm_sym_jmp + addChar + regex_pc_imm_jmp + addChar + regex_indirect_sum_lit_jmp + addChar + regex_indirect_sum_sym_jmp + addChar + regex_mem_lit_jmp + addChar + regex_mem_sym_jmp;
#define regex_3B_jump regex(jump3B)
#define regex_5B_jump regex(jump5B)

#define regex_special_character regex("^((\\$)|(%)|(\\*))([a-zA-z]+)\\s*")
#define regex_register regex("^r[0-7]\\s*")
