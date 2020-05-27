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

#endif
