#ifndef EM_PROC_H
#define EM_PROC_H

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 64000
#define GENERAL_REGISTERS_NUM 13

struct Machine {
	uint8_t memory[MEMORY_SIZE];
	uint32_t general_reg[GENERAL_REGISTERS_NUM];
	uint32_t cpsr_reg;
	uint32_t pc_reg;
	bool end;
};
typedef struct Machine Machine;

struct Decoded_Instr {
	bool exists;
	uint8_t cond;
	uint8_t type; // 4 main types of instr
	uint8_t rn;
	uint8_t rd;
	uint32_t offset;

	// data processing
	uint32_t op2;
	bool set;

	//multiply
	uint32_t rm_val;
	uint32_t rs_val;

	// single data transferr
	bool imm;
	bool pre;
	bool load;
	bool up;
};
typedef struct Decoded_Instr Decoded_Instr;

struct Instr {
	bool exists;
	uint32_t instr;
};
typedef struct Instr Instr;

// Execute the decoded instruction 'instr'

void execute(Decoded_Instr *instr,Machine *arm);

// Decode the fetched 'instr' and store it in 'decoded'

void decode (Decoded_Instr *decoded,Instr *instr,Machine *arm);

// TODO: actual bit-wise and calculation functions

#endif
