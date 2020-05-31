#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "emulator_processor.h"
#include "decode_helpers.h"
#include "define_structures.h"

#define TRUE 420


void print_machine_status(Machine *arm){
	printf("Registers:\n");
	for(int i = 0; i < GENERAL_REGISTERS_NUM; i++) {
		printf("$%-2d : %10d (0x%08x)\n",i,arm->general_reg[i],arm->general_reg[i]);
	}
	printf("PC  : %*d (0x%08x)\n",10,arm->pc_reg,arm->pc_reg);
	printf("CPSR: %*d (0x%08x)\n",10,arm->cpsr_reg,arm->cpsr_reg);

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

int main(int argc, char **argv) {

	// Argument check and file read
	//
	FILE *input;

	if (argc != 2) {
		fprintf(stderr,"Invalid argument number");
		exit(EXIT_FAILURE);
	}
	else {
		if((input = fopen(argv[1],"rb")) == NULL) {
			fprintf(stderr,"File could not be found");
			exit(EXIT_FAILURE);
		}
	}

	// -- Declaration and initialization of memory and registers storage
	// --

	Machine arm;

	memset(arm.memory, 0, MEMORY_SIZE * sizeof(uint8_t));
	memset(arm.general_reg, 0, GENERAL_REGISTERS_NUM * sizeof(uint32_t));

	arm.cpsr_reg = 0;
	arm.pc_reg = 0;
	arm.end = false;
	arm.branch_executed = false;
	arm.shifter_carry = 0;

	// -- Store input onto memory
	// -- byte-by-byte and then divide count by 4

	int instr_count = 0;

	// Functions pointer array

	ProcFunc data_proc_func[14];
	init_data_proc_func(data_proc_func);

	while(!feof(input)) {
		if(instr_count == MEMORY_SIZE) {
			fprintf(stderr,"Instructions exceeded memory size");
			exit(EXIT_FAILURE);
		}
		fread(&arm.memory[instr_count],1,1,input);
		++instr_count;
	}

	instr_count = ceil(instr_count / 4);

	// -- Start the pipeline
	// -- initialise all 3 stages to NULL

	Instr fetched_instr;
	fetched_instr.exists = false;
	fetched_instr.bits = 0;
	Decoded_Instr decoded_instr;

	while(!arm.end) {
		if(decoded_instr.exists) {
			execute(&decoded_instr,&arm,data_proc_func); // <- execute previously decoded instruction
			if(arm.end) {
				break; // <- exit loop if halt instruction was executed
			}
			if(arm.branch_executed) {
				decoded_instr.exists = false;
				fetched_instr.exists = false; // <- if branch instruction was executed
				arm.branch_executed = false;  // <- clear the pipeline and the checker
			}
		}

		if(fetched_instr.exists) {
			decode(&decoded_instr,&fetched_instr,&arm); // <- decode previously fetched instruction
		}

		if(abs(arm.pc_reg-PIPELINE_OFFSET) >= MEMORY_SIZE) {
			perror("PC exceeded memory size");
			exit(EXIT_FAILURE);
		}
		memcpy(&fetched_instr.bits,&arm.memory[arm.pc_reg],4); // fetch from memory acc to PC
		fetched_instr.exists = true;

		if(fetched_instr.exists && fetched_instr.bits) {
//			print_bits(fetched_instr.bits);
		}
		arm.pc_reg += 4;
	}

	// -- Print the final machine state
	// --

	print_machine_status(&arm);

	return EXIT_SUCCESS;
}
