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
#include <string.h>

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

// -- FOR ALL TYPES OF INSTRUCTIONS

// return condition bits
uint32_t get_cond(Instr *instruction) {
	return (instruction->bits) >> COND_POS;
}

// return condition type
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


// -- DATA PROCESSING ONLY
// return Opcode
uint32_t get_opcode(Instr *instruction) {
	return ((OPCODE_MASK << OPCODE_POS) & instruction->bits) >> OPCODE_POS;
}

// return Operand2
uint32_t get_operand2(Instr *instruction) {
	return (OPERAND2_MASK & instruction->bits);
}

// -- MULTIPLY ONLY
// check if it should accumulate
bool to_accumulate(Instr *instruction) {
	return (ACCUMULATE & instruction->bits);
}

// return Rn
uint32_t get_rn_MUL(Instr *instruction) {
	return ((RN_MUL_MASK << RN_MUL_POS) & instruction->bits) >> RN_MUL_POS;
}

// return Rd
uint32_t get_rd_MUL(Instr *instruction) {
	return ((RD_MUL_MASK << RD_MUL_POS) & instruction->bits) >> RD_MUL_POS;
}

// return Rs
uint32_t get_rs_MUL(Instr *instruction) {
	return ((RS_MUL_MASK << RS_MUL_POS) & instruction->bits) >> RS_MUL_POS;
}

// return Rm
uint32_t get_rm_MUL(Instr *instruction) {
	return (RM_MUL_MASK & instruction->bits);
}


// -- SINGLE DATA TRANSFER ONLY
// check Pre/Post-indexing bit
bool is_pre_index(Instr *instruction) {
	return (P_INDEXING_MASK & instruction->bits);
}

// check Up bit
bool is_up(Instr *instruction) {
	return (UP_MASK & instruction->bits);
}

// check Load bit
bool is_load(Instr *instruction) {
	return (LOAD_MASK & instruction->bits);
}

// getOffset
uint32_t get_offset_TRANSFER(Instr *instruction) {
	return (OFFSET_TRANSFER_MASK & instruction->bits);
}

// -- BRANCH ONLY
// get Offset
int32_t get_offset_BRANCH(Instr *instruction) {
	return (OFFSET_BRANCH_MASK & instruction->bits);
}

// -- DATA PROCESSING AND MULTIPLY ONLY
// check Immediate bit
bool is_immediate(Instr *instruction) {
	return (IMMEDIATE_MASK & instruction->bits);
}

// return Rn
uint32_t get_rn(Instr *instruction) {
	return ((RN_MASK << RN_POS) & instruction->bits) >> RN_POS;
}

// return Rd
uint32_t get_rd(Instr *instruction) {
	return ((RD_MASK << RD_POS) & instruction->bits) >> RD_POS;
}


// -- DATA PROCESSING AND SINGLE DATA TRANSFER ONLY
// check Set Condition Codes bit
bool is_set(Instr *instruction) {
	return (SET_COND_CODES & instruction->bits);
}

// -- PRINTING HELPERS
//

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
		uint8_t rotate = (offset & ROTATE_MASK) >> 8;
		op2 = barrel_shift(op2,2*rotate,ROTATE_RIGHT,arm);
	}
	else {
		// shifter register value
		uint32_t shift = (offset & SHIFT_MASK) >> 4;
		uint8_t rm = offset & RM_MASK;
		uint32_t val = arm->general_reg[rm];

		if(!(shift & 1)) {
			// constant ammounts
			uint8_t amm = ((0x1f << 3) & shift) >> 3;
			uint8_t type = ((0x3 << 1) & shift) >> 1;
			op2 = barrel_shift(val,amm,type,arm);
		}
		else {
			// ammount specifies by a register
			uint8_t rs = ((0xf << 4) & shift) >> 4;
			uint8_t type = ((0x3 << 1) & shift) >> 1;
			uint8_t amm = arm->general_reg[rs] & 0xff;
			op2 = barrel_shift(val,amm,type,arm);
		}
	}
	return op2;
}

void print_instr(Decoded_Instr *instr){
	switch (instr->type) {
	case HALT:
		printf("EXIT...\n");
		return;
	case DATA_PROC:
		printf("DATA PROCESSING\n");
		printf("COND:%x ",instr->cond);
		printf("RN:%x ",instr->rn);
		printf("RD:%x ",instr->rd);
		printf("IMM:%x ",instr->imm);
		printf("OP2:%x ",instr->op2);
		printf("OFFSET_OP2: ");
		print_bits(instr->offset);
		printf("OPCODE:%x ",instr->opcode);
		printf("SET:%x ",instr->set);
		return;
	case MUL:
		printf("MULTIPLY\n");
		printf("COND:%x ",instr->cond);
		printf("RN:%x ",instr->rn);
		printf("RD:%x ",instr->rd);
		printf("RM:%x ",instr->op2);
		printf("RS:%x ",instr->opcode);
		printf("ACCUM:%x ",instr->accum);
		printf("SET:%x ",instr->set);
		return;
	case TRANSFER:
		printf("DATA TRANSFER\n");
		printf("COND:%x ",instr->cond);
		printf("RN:%x ",instr->rn);
		printf("RD:%x ",instr->rd);
		printf("IMM:%x ",instr->imm);
		printf("PRE:%x ",instr->pre_index);
		printf("SET:%x ",instr->set);
		printf("LOAD:%x ",instr->load);
		printf("UP:%x ",instr->up);
		printf("OFFSET:%x ",instr->offset);
		return;
	case BRANCH:
		printf("BRANCH\n");
		printf("COND:%x",instr->cond);
		printf("OFFSET:%x ",instr->offset);
		return;
	case NOOP:
		return;
	}
}

// print machine status according to test format

void print_machine_status(Machine *arm){
	printf("Registers:\n");
	for(int i = 0; i < GENERAL_REGISTERS_NUM; i++) {
		printf("$%-2d : %10d (0x%08x)\n",i,arm->general_reg[i],arm->general_reg[i]);
	}
	printf("PC  : %*d (0x%08x)\n",10,arm->pc_reg,arm->pc_reg);
	printf("CPSR: %*d (0x%08x)\n",10,arm->cpsr_reg,arm->cpsr_reg);
//	printf("CARRY: (0x%08x)\n",arm->shifter_carry);

	int val;
	printf("Non-zero memory:\n");
	for(int i = 0; i < MEMORY_SIZE / 4; i++) {
		memcpy(&val,&arm->memory[4 * i],4);
		uint8_t b0 = arm->memory[4 * i];
		uint8_t b1 = arm->memory[4 * i + 1];
		uint8_t b2 = arm->memory[4 * i + 2];
		uint8_t b3 = arm->memory[4 * i + 3];
		if(val) {
			printf("0x%08x: 0x%02x%02x%02x%02x\n",4*i,b0,b1,b2,b3);
		}
	}
}
