//
// Created by bianca on 5/28/20.
//
#include "decode_helpers.h"
#include "define_structures.h"
# include "emulator_processor.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//Data processing

#define IMM_MASK 0xff
#define ROTATE_MASK (0xf << 8)
#define SHIFT_MASK (0xff << 4)
#define RM_MASK 0xf

// Barrel shifter options

#define LOGICAL_LEFT 0
#define LOGICAL_RIGHT 1
#define ARITHM_RIGHT 2
#define ROTATE_RIGHT 3

//FUNCTIONS:

//FOR ALL TYPES OF INSTRUCTIONS
//return condition bits
uint32_t get_cond(Instr *instruction) {
	return (instruction->bits) >> COND_POS;
}

//return condition type
enum type get_instr_type(Instr *instruction) {
	if(instruction->bits == 0) {
		return HALT;
	}
	if ((1 << 27) & instruction->bits) {
		return BRANCH;
	}
	if ((1 << 26) & instruction->bits) {
		return TRANSFER;
	}
	if (IMMEDIATE_MASK & instruction->bits) {
		return DATA_PROC;
	}
	if (!((1 << 4) & instruction->bits)) {
		return DATA_PROC;
	}
	if (!((1 << 7) & instruction->bits)) {
		return DATA_PROC;
	}
	return MUL;
}


//FOR DATA PROCESSING ONLY
//return Opcode
uint32_t get_opcode(Instr *instruction) {
	return ((OPCODE_MASK << OPCODE_POS) & instruction->bits) >> OPCODE_POS;
}

//return Operand2
uint32_t get_operand2(Instr *instruction) {
	return (OPERAND2_MASK & instruction->bits);
}

//FOR MULTIPLY ONLY
//check if it should accumulate
bool to_accumulate(Instr *instruction) {
	return (ACCUMULATE & instruction->bits);
}

//return Rn
uint32_t get_rn_MUL(Instr *instruction) {
	return ((RN_MUL_MASK << RN_MUL_POS) & instruction->bits) >> RN_MUL_POS;
}

// return Rd
uint32_t get_rd_MUL(Instr *instruction) {
	return ((RD_MUL_MASK << RD_MUL_POS) & instruction->bits) >> RD_MUL_POS;
}

//return Rs
uint32_t get_rs_MUL(Instr *instruction) {
	return ((RS_MUL_MASK << RS_MUL_POS) & instruction->bits) >> RS_MUL_POS;
}

//Return Rm
uint32_t get_rm_MUL(Instr *instruction) {
	return (RM_MUL_MASK & instruction->bits);
}


//FOR SINGLE DATA TRANSFER ONLY
//check Pre/Post-indexing bit
bool is_pre_index(Instr *instruction) {
	return (P_INDEXING_MASK & instruction->bits);
}

//check Up bit
bool is_up(Instr *instruction) {
	return (UP_MASK & instruction->bits);
}

//check Load bit
bool is_load(Instr *instruction) {
	return (LOAD_MASK & instruction->bits);
}

//getOffset
uint32_t get_offset_TRANSFER(Instr *instruction) {
	return (OFFSET_TRANSFER_MASK & instruction->bits);
}


//FOR BRANCH ONLY
//get Offset
int32_t get_offset_BRANCH(Instr *instruction) {
	return (OFFSET_BRANCH_MASK & instruction->bits);
}

//FOR DATA PROCESSING AND MULTIPLY ONLY
//check Immediate bit
bool is_immediate(Instr *instruction) {
	return (IMMEDIATE_MASK & instruction->bits);
}

//return Rn
uint32_t get_rn(Instr *instruction) {
	return ((RN_MASK << RN_POS) & instruction->bits) >> RN_POS;
}

//return Rd
uint32_t get_rd(Instr *instruction) {
	return ((RD_MASK << RD_POS) & instruction->bits) >> RD_POS;
}


//FOR DATA PROCESSING AND SINGLE DATA TRANSFER ONLY
//check Set Condition Codes bit
bool is_set(Instr *instruction) {
	return (SET_COND_CODES & instruction->bits);
}

void print_bits(uint32_t val){
	uint32_t mask = 1 << 31;
	for(int i = 0; i < 32; i++) {
		int b = val & mask;
		printf("%d",b != 0);
		mask = mask >> 1;
	}
	printf("\n");
}

uint32_t decode_offset(uint32_t offset,bool imm,Machine* arm){
	uint32_t op2;
	if(imm) {
		// immediate value
		op2 = offset & IMM_MASK;
		uint8_t rotate = offset & ROTATE_MASK;
		op2 = barrel_shift(op2,2*rotate,ROTATE_RIGHT,arm);
	}
	else {
		// shifter register value
		uint8_t shift = offset & SHIFT_MASK;
		uint8_t rm = offset & SHIFT_MASK;
		uint32_t val = arm->general_reg[rm];

		if(shift & 1) {
			// constant ammount
			uint8_t amm = (0xf1 << 3) & shift;
			uint8_t type = (0x3 << 1) & shift;
			op2 = barrel_shift(val,amm,type,arm);
		}
		else {
			// ammount specifies by a register
			uint8_t rs = (0xf << 4) & shift;
			uint8_t type = (0x3 << 1) & shift;
			uint8_t amm = arm->general_reg[rs] & 0xff;
			op2 = barrel_shift(val,amm,type,arm);
		}
	}
	return op2;
}
