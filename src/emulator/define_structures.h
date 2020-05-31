//
// Created by bianca on 5/28/20.
//

#ifndef ARM11_18_DEFINE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define ARM11_18_DEFINE_TYPES_H
#define MEMORY_SIZE 64000
#define GENERAL_REGISTERS_NUM 13

#define PIPELINE_OFFSET 8

struct Machine {
	uint8_t memory[MEMORY_SIZE];
	uint32_t general_reg[GENERAL_REGISTERS_NUM];
	uint32_t cpsr_reg;
	uint32_t pc_reg;
	bool end;
	bool branch_executed;
	uint8_t shifter_carry;
};
typedef struct Machine Machine;

typedef uint32_t (*ProcFunc)(uint32_t,uint32_t,bool,Machine*);

enum cond {
	eq = 0x0, // 0000
	ne = 0x1, // 0001
	ge = 0xa, // 1010
	lt = 0xb, // 1011
	gt = 0xc, // 1100
	le = 0xd, // 1101
	al = 0xe  // 1110
};

enum type {
	HALT,
	DATA_PROC,
	MUL,
	TRANSFER,
	BRANCH,
	NOOP = 0xffff // was 0xffffffff but enum should be int (32 bit)
};
struct Decoded_Instr {
	bool exists;
	enum cond cond;
	enum type type;   // 4 main types of instr + noop
	uint8_t rn;
	uint8_t rd;
	uint32_t offset;
	bool set;

	// data processing
	uint8_t opcode;
	uint32_t op2;

	//multiply
	bool accum;
	uint32_t rm;
	uint32_t rs;

	// single data transfer
	bool imm;
	bool pre_index;
	bool load;
	bool up;

	// branch
	int32_t sgn_offset;
};
typedef struct Decoded_Instr Decoded_Instr;

struct Instr {
	bool exists;
	uint32_t bits;
};
typedef struct Instr Instr;

#endif //ARM11_18_DEFINE_TYPES_H
