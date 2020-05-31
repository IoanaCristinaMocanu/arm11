//
// Created by bianca on 5/28/20.
//

#ifndef ARM11_18_DECODE_HELPERS_H
#define ARM11_18_DECODE_HELPERS_H
#include "define_structures.h"

//CONSTANTS:

//for all types of instruction
#define COND_POS 28

//for Data Processing ONLY
#define OPERAND2_MASK 0xfff // for bits 0-11
#define OPCODE_MASK 0xf //for bits 21-24
#define OPCODE_POS 21

//for Multiply ONLY
#define ACCUMULATE 1 << 21
#define RD_MUL_MASK 0xf //for bits 16-19
#define RN_MUL_MASK 0xf //for bits 12-15
#define RS_MUL_MASK 0xf // for bits 8-11
#define RM_MUL_MASK 0xf //for bits 0-3
#define RD_MUL_POS 16
#define RN_MUL_POS 12
#define RS_MUL_POS 8

//for Single Data Transfer ONLY
#define P_INDEXING_MASK 1 << 24
#define UP_MASK 1 << 23
#define LOAD_MASK 1 << 20
#define OFFSET_TRANSFER_MASK 0xfff // for bits 0-11

//for Branch ONLY
#define OFFSET_BRANCH_MASK 0xFFFFFF //for bits 0-23

//for Data Processing and Single Data Transfer
#define IMMEDIATE_MASK 1 << 25
#define RN_MASK 0xf //for bits 16-19
#define RD_MASK 0xf //for bits 12-15
#define RN_POS 16
#define RD_POS 12

//for Data Processing and Multiply
#define SET_COND_CODES 1 << 20


//FUNCTIONS:

//FOR ALL TYPES OF INSTRUCTIONS
//return condition bits
uint32_t get_cond(Instr *instruction);

//return condition type
enum type get_instr_type(Instr *instruction);


//FOR DATA PROCESSING ONLY
//return Opcode
uint32_t get_opcode(Instr *instruction);

//return Operand2
uint32_t get_operand2(Instr *instruction);


//FOR MULTIPLY ONLY
//check if it should accumulate
bool to_accumulate(Instr *instruction);

//return Rn
uint32_t get_rn_MUL(Instr *instruction);

// return Rd
uint32_t get_rd_MUL(Instr *instruction);

//return Rs
uint32_t get_rs_MUL(Instr *instruction);

//Return Rm
uint32_t get_rm_MUL(Instr *instruction);


//FOR SINGLE DATA TRANSFER ONLY
//check Pre/Post-indexing bit
bool is_pre_index(Instr *instruction);

//check Up bit
bool is_up(Instr *instruction);

//check Load bit
bool is_load(Instr *instruction);

//getOffset
uint32_t get_offset_TRANSFER(Instr *instruction);


//FOR BRANCH ONLY
//get Offset
int32_t get_offset_BRANCH(Instr *instruction);


//FOR DATA PROCESSING AND MULTIPLY ONLY
//check Immediate bit
bool is_immediate(Instr *instruction);

//return Rn
uint32_t get_rn(Instr *instruction);

//return Rd
uint32_t get_rd(Instr *instruction);


//FOR DATA PROCESSING AND SINGLE DATA TRANSFER ONLY
//check Set Condition Codes bit
bool is_set(Instr *instruction);

void print_bits(uint32_t val);

uint32_t decode_offset(uint32_t offset,bool imm,Machine* arm);

#endif //ARM11_18_DECODE_HELPERS_H
