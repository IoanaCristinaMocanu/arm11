//
// Created by bianca on 5/28/20.
//

#ifndef ARM11_18_DECODE_HELPERS_H
#define ARM11_18_DECODE_HELPERS_H
#include "define_structures.h"

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


// FOR DATA PROCESSING AND SINGLE DATA TRANSFER ONLY
// check Set Condition Codes bit
bool is_set(Instr *instruction);

// -- decodes the shifted register value provided in data processing
// and data transfer instructions

uint32_t decode_offset(uint32_t offset,bool imm,Machine* arm);

// --
// -- Printing and debug helpers
// --

void print_bits(uint32_t val);

void print_instr(Decoded_Instr *instr);

void print_machine_status(Machine *arm);

#endif //ARM11_18_DECODE_HELPERS_H
